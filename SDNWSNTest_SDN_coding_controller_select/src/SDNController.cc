#include "SDNController.h"
#include <queue>
#include <cmath>
#include <algorithm>

Define_Module(SDNController);

void SDNController::initialize()
{
    numSensors = getParentModule()->par("numSensors");
    numNodes = getParentModule()->par("numNodes");
    sinkId = numSensors;

    nextHopTable.assign(numNodes, -1);
    depthToSink.assign(numNodes, -1);
    childrenCount.assign(numNodes, 0);
    subtreeSize.assign(numNodes, 1);
    codingRelayTable.assign(numNodes, false);

    buildRoutingTable();
    analyzeRoutingTree();
    selectCodingRelays();

    EV << "SDN Controller started. Routing/coding tables installed." << endl;
    for (int i = 0; i < numNodes; i++) {
        EV << "Node " << i
           << " -> nextHop=" << nextHopTable[i]
           << " depth=" << depthToSink[i]
           << " children=" << childrenCount[i]
           << " subtreeSize=" << subtreeSize[i]
           << " codingRelay=" << codingRelayTable[i]
           << endl;
    }
}

void SDNController::handleMessage(cMessage *msg)
{
    delete msg;
}

void SDNController::finish()
{
    int reachable = 0;
    int codingRelays = 0;

    for (int i = 0; i < numNodes; i++) {
        if (i == sinkId || nextHopTable[i] >= 0)
            reachable++;
        if (codingRelayTable[i])
            codingRelays++;
    }

    recordScalar("reachableNodes", reachable);
    recordScalar("selectedCodingRelays", codingRelays);
}

double SDNController::distanceBetween(int a, int b) const
{
    cModule *ma = getParentModule()->getSubmodule("node", a);
    cModule *mb = getParentModule()->getSubmodule("node", b);

    double ax = ma->par("xCoord").doubleValue();
    double ay = ma->par("yCoord").doubleValue();
    double bx = mb->par("xCoord").doubleValue();
    double by = mb->par("yCoord").doubleValue();

    double dx = ax - bx;
    double dy = ay - by;
    return std::sqrt(dx * dx + dy * dy);
}

void SDNController::buildRoutingTable()
{
    std::vector<bool> visited(numNodes, false);
    std::queue<int> q;

    visited[sinkId] = true;
    nextHopTable[sinkId] = -1;
    depthToSink[sinkId] = 0;
    q.push(sinkId);

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        for (int n = 0; n < numNodes; n++) {
            if (n == current)
                continue;
            if (visited[n])
                continue;

            cModule *node = getParentModule()->getSubmodule("node", n);
            double range = node->par("radioRange").doubleValue();

            if (distanceBetween(current, n) <= range) {
                visited[n] = true;
                nextHopTable[n] = current;
                depthToSink[n] = depthToSink[current] + 1;
                q.push(n);
            }
        }
    }
}

void SDNController::analyzeRoutingTree()
{
    std::fill(childrenCount.begin(), childrenCount.end(), 0);
    std::fill(subtreeSize.begin(), subtreeSize.end(), 1);

    int maxDepth = 0;

    for (int i = 0; i < numNodes; i++) {
        int parent = nextHopTable[i];
        if (parent >= 0 && parent < numNodes)
            childrenCount[parent]++;

        if (depthToSink[i] > maxDepth)
            maxDepth = depthToSink[i];
    }

    for (int d = maxDepth; d >= 1; --d) {
        for (int i = 0; i < numNodes; i++) {
            if (depthToSink[i] == d) {
                int parent = nextHopTable[i];
                if (parent >= 0)
                    subtreeSize[parent] += subtreeSize[i];
            }
        }
    }
}

void SDNController::selectCodingRelays()
{
    std::fill(codingRelayTable.begin(), codingRelayTable.end(), false);

    for (int i = 0; i < numNodes; i++) {
        if (i == sinkId)
            continue;
        if (nextHopTable[i] < 0)
            continue;  // unreachable node

        // coding only at useful convergence relays
        if (childrenCount[i] >= 2 && depthToSink[i] >= 2) {
            codingRelayTable[i] = true;
        }
    }
}

int SDNController::getNextHop(int nodeId) const
{
    if (nodeId < 0 || nodeId >= (int)nextHopTable.size())
        return -1;
    return nextHopTable[nodeId];
}

bool SDNController::isCodingRelay(int nodeId) const
{
    if (nodeId < 0 || nodeId >= (int)codingRelayTable.size())
        return false;
    return codingRelayTable[nodeId];
}

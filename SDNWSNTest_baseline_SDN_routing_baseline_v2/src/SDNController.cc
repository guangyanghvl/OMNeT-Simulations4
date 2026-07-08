#include "SDNController.h"
#include <queue>
#include <cmath>

Define_Module(SDNController);

void SDNController::initialize()
{
    numSensors = getParentModule()->par("numSensors");
    numNodes = getParentModule()->par("numNodes");
    sinkId = numSensors;

    nextHopTable.assign(numNodes, -1);
    buildRoutingTable();

    EV << "SDN Controller started. Routing table installed." << endl;
    for (int i = 0; i < numNodes; i++) {
        EV << "Route node " << i << " -> next hop " << nextHopTable[i] << endl;
    }
}

void SDNController::handleMessage(cMessage *msg)
{
    delete msg;
}

void SDNController::finish()
{
    int installed = 0;
    for (int i = 0; i < numNodes; i++) {
        if (nextHopTable[i] >= 0)
            installed++;
    }
    recordScalar("installedRoutes", installed);
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
                q.push(n);
            }
        }
    }
}

int SDNController::getNextHop(int nodeId) const
{
    if (nodeId < 0 || nodeId >= (int)nextHopTable.size())
        return -1;
    return nextHopTable[nodeId];
}

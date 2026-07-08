#include "WSNNode.h"
#include <sstream>

Define_Module(WSNNode);

long WSNNode::totalGenerated = 0;
long WSNNode::totalForwarded = 0;
long WSNNode::totalReceived = 0;
long WSNNode::totalDroppedNoNextHop = 0;
long WSNNode::totalDroppedTtl = 0;
long WSNNode::totalDroppedDuplicate = 0;
long WSNNode::sinkHopCountSum = 0;
double WSNNode::sinkDelaySum = 0;

double WSNNode::totalNetworkEnergy = 0;
double WSNNode::totalNetworkGenEnergy = 0;
double WSNNode::totalNetworkRxEnergy = 0;
double WSNNode::totalNetworkTxEnergy = 0;
double WSNNode::totalNetworkCodingEnergy = 0;

void WSNNode::initialize()
{
    nodeId = par("nodeId");
    isSink = par("isSink").boolValue();
    isController = par("isController").boolValue();
    sinkId = par("sinkId");
    xCoord = par("xCoord").doubleValue();
    yCoord = par("yCoord").doubleValue();
    radioRange = par("radioRange").doubleValue();
    sendInterval = par("sendInterval");
    initialTtl = par("initialTtl");
    seqCounter = 0;
    sendTimer = nullptr;

    generatedCount = 0;
    forwardedCount = 0;
    receivedCount = 0;
    droppedNoNextHopCount = 0;
    droppedTtlCount = 0;
    droppedDuplicateCount = 0;

    generationEnergyCost = par("generationEnergyCost").doubleValue();
    rxEnergyCost = par("rxEnergyCost").doubleValue();
    txEnergyCost = par("txEnergyCost").doubleValue();
    codingEnergyCost = par("codingEnergyCost").doubleValue();

    energyGen = 0;
    energyRx = 0;
    energyTx = 0;
    energyCoding = 0;
    totalEnergy = 0;

    if (nodeId == 0) {
        totalGenerated = 0;
        totalForwarded = 0;
        totalReceived = 0;
        totalDroppedNoNextHop = 0;
        totalDroppedTtl = 0;
        totalDroppedDuplicate = 0;
        sinkHopCountSum = 0;
        sinkDelaySum = 0;

        totalNetworkEnergy = 0;
        totalNetworkGenEnergy = 0;
        totalNetworkRxEnergy = 0;
        totalNetworkTxEnergy = 0;
        totalNetworkCodingEnergy = 0;
    }

    if (!isSink && !isController) {
        sendTimer = new cMessage("sendTimer");
        scheduleAt(simTime() + uniform(0, 1), sendTimer);
    }

    EV << "Node " << nodeId
       << " x=" << xCoord
       << " y=" << yCoord
       << " sink=" << isSink
       << " controller=" << isController << endl;
}

void WSNNode::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        generatePacket();
        scheduleAt(simTime() + sendInterval, msg);
        return;
    }

    WSNPacket *pkt = check_and_cast<WSNPacket *>(msg);

    energyRx += rxEnergyCost;
    totalEnergy += rxEnergyCost;
    totalNetworkRxEnergy += rxEnergyCost;
    totalNetworkEnergy += rxEnergyCost;

    std::string key = makePacketKey(pkt);

    if (seenPackets.find(key) != seenPackets.end()) {
        droppedDuplicateCount++;
        totalDroppedDuplicate++;
        EV << "Node " << nodeId << " dropping duplicate packet " << key << endl;
        delete pkt;
        return;
    }

    seenPackets.insert(key);

    if (isSink && nodeId == pkt->getFinalDstId()) {
        receivedCount++;
        totalReceived++;
        sinkHopCountSum += pkt->getHopCount();
        sinkDelaySum += SIMTIME_DBL(simTime() - pkt->getCreatedAt());

        EV << "Sink received packet from src=" << pkt->getSrcId()
           << " seq=" << pkt->getSeq()
           << " hops=" << pkt->getHopCount()
           << " delay=" << (simTime() - pkt->getCreatedAt())
           << " ttl=" << pkt->getTtl() << endl;
        delete pkt;
        return;
    }

    if (pkt->getTtl() <= 0) {
        droppedTtlCount++;
        totalDroppedTtl++;
        EV << "Node " << nodeId << " dropping packet due to TTL expiry"
           << " src=" << pkt->getSrcId()
           << " seq=" << pkt->getSeq() << endl;
        delete pkt;
        return;
    }

    forwardPacket(pkt);
}

void WSNNode::generatePacket()
{
    WSNPacket *pkt = new WSNPacket("data");
    pkt->setSrcId(nodeId);
    pkt->setLastHopId(nodeId);
    pkt->setFinalDstId(sinkId);
    pkt->setSeq(seqCounter++);
    pkt->setHopCount(0);
    pkt->setTtl(initialTtl);
    pkt->setCreatedAt(simTime());

    generatedCount++;
    totalGenerated++;

    energyGen += generationEnergyCost;
    totalEnergy += generationEnergyCost;
    totalNetworkGenEnergy += generationEnergyCost;
    totalNetworkEnergy += generationEnergyCost;

    seenPackets.insert(makePacketKey(pkt));

    EV << "Node " << nodeId
       << " generated packet seq=" << pkt->getSeq()
       << " ttl=" << pkt->getTtl() << endl;

    forwardPacket(pkt);
}

void WSNNode::forwardPacket(WSNPacket *pkt)
{
    int nextHop = chooseNextHop();

    if (nextHop < 0) {
        droppedNoNextHopCount++;
        totalDroppedNoNextHop++;
        EV << "Node " << nodeId << " dropping packet: no next hop"
           << " src=" << pkt->getSrcId()
           << " seq=" << pkt->getSeq() << endl;
        delete pkt;
        return;
    }

    pkt->setLastHopId(nodeId);
    pkt->setHopCount(pkt->getHopCount() + 1);
    pkt->setTtl(pkt->getTtl() - 1);

    forwardedCount++;
    totalForwarded++;

    energyTx += txEnergyCost;
    totalEnergy += txEnergyCost;
    totalNetworkTxEnergy += txEnergyCost;
    totalNetworkEnergy += txEnergyCost;

    EV << "Node " << nodeId
       << " forwarding packet src=" << pkt->getSrcId()
       << " seq=" << pkt->getSeq()
       << " ttl=" << pkt->getTtl()
       << " to node " << nextHop << endl;

    sendDirect(pkt, getParentModule()->getSubmodule("node", nextHop), "in");
}

double WSNNode::distanceTo(int otherIndex)
{
    cModule *other = getParentModule()->getSubmodule("node", otherIndex);
    double ox = other->par("xCoord").doubleValue();
    double oy = other->par("yCoord").doubleValue();
    double dx = xCoord - ox;
    double dy = yCoord - oy;
    return std::sqrt(dx * dx + dy * dy);
}

double WSNNode::distanceToSink(int otherIndex)
{
    cModule *sink = getParentModule()->getSubmodule("node", sinkId);
    double sx = sink->par("xCoord").doubleValue();
    double sy = sink->par("yCoord").doubleValue();

    cModule *other = getParentModule()->getSubmodule("node", otherIndex);
    double ox = other->par("xCoord").doubleValue();
    double oy = other->par("yCoord").doubleValue();

    double dx = ox - sx;
    double dy = oy - sy;
    return std::sqrt(dx * dx + dy * dy);
}

int WSNNode::chooseNextHop()
{
    if (nodeId == sinkId)
        return -1;

    int n = getParentModule()->par("numNodes");
    double mySinkDist = distanceToSink(nodeId);

    int bestNode = -1;
    double bestDist = mySinkDist;

    for (int i = 0; i < n; i++) {
        if (i == nodeId)
            continue;

        cModule *cand = getParentModule()->getSubmodule("node", i);
        bool candIsController = cand->par("isController").boolValue();
        if (candIsController)
            continue;

        double d = distanceTo(i);
        if (d > radioRange)
            continue;

        double candSinkDist = distanceToSink(i);
        if (candSinkDist < bestDist) {
            bestDist = candSinkDist;
            bestNode = i;
        }
    }

    return bestNode;
}

std::string WSNNode::makePacketKey(WSNPacket *pkt) const
{
    std::ostringstream os;
    os << pkt->getSrcId() << "-" << pkt->getSeq();
    return os.str();
}

void WSNNode::finish()
{
    if (sendTimer)
        cancelAndDelete(sendTimer);

    recordScalar("generatedPackets", generatedCount);
    recordScalar("forwardedPackets", forwardedCount);
    recordScalar("receivedPackets", receivedCount);
    recordScalar("droppedNoNextHop", droppedNoNextHopCount);
    recordScalar("droppedTtl", droppedTtlCount);
    recordScalar("droppedDuplicate", droppedDuplicateCount);

    recordScalar("energyGeneration", energyGen);
    recordScalar("energyReception", energyRx);
    recordScalar("energyTransmission", energyTx);
    recordScalar("energyCoding", energyCoding);
    recordScalar("energyTotal", totalEnergy);

    if (nodeId == 0) {
        double deliveryRatio = totalGenerated > 0 ? (double)totalReceived / totalGenerated : 0.0;
        double avgHopCount = totalReceived > 0 ? (double)sinkHopCountSum / totalReceived : 0.0;
        double avgDelay = totalReceived > 0 ? sinkDelaySum / totalReceived : 0.0;

        double energyPerDeliveredPacket = totalReceived > 0 ? totalNetworkEnergy / totalReceived : 0.0;

        recordScalar("networkTotalGenerated", totalGenerated);
        recordScalar("networkTotalForwarded", totalForwarded);
        recordScalar("networkTotalReceived", totalReceived);
        recordScalar("networkDroppedNoNextHop", totalDroppedNoNextHop);
        recordScalar("networkDroppedTtl", totalDroppedTtl);
        recordScalar("networkDroppedDuplicate", totalDroppedDuplicate);
        recordScalar("networkDeliveryRatio", deliveryRatio);
        recordScalar("networkAverageHopCount", avgHopCount);
        recordScalar("networkAverageDelay", avgDelay);

        recordScalar("networkTotalEnergy", totalNetworkEnergy);
        recordScalar("networkGenerationEnergy", totalNetworkGenEnergy);
        recordScalar("networkReceptionEnergy", totalNetworkRxEnergy);
        recordScalar("networkTransmissionEnergy", totalNetworkTxEnergy);
        recordScalar("networkCodingEnergy", totalNetworkCodingEnergy);
        recordScalar("networkEnergyPerDeliveredPacket", energyPerDeliveredPacket);

        EV << "=== Network Summary ===" << endl;
        EV << "Total generated: " << totalGenerated << endl;
        EV << "Total forwarded: " << totalForwarded << endl;
        EV << "Total received: " << totalReceived << endl;
        EV << "Drop no next hop: " << totalDroppedNoNextHop << endl;
        EV << "Drop TTL: " << totalDroppedTtl << endl;
        EV << "Drop duplicate: " << totalDroppedDuplicate << endl;
        EV << "Delivery ratio: " << deliveryRatio << endl;
        EV << "Average hop count: " << avgHopCount << endl;
        EV << "Average delay: " << avgDelay << endl;
        EV << "Total network energy: " << totalNetworkEnergy << endl;
        EV << "Generation energy: " << totalNetworkGenEnergy << endl;
        EV << "Reception energy: " << totalNetworkRxEnergy << endl;
        EV << "Transmission energy: " << totalNetworkTxEnergy << endl;
        EV << "Coding energy: " << totalNetworkCodingEnergy << endl;
        EV << "Energy per delivered packet: " << energyPerDeliveredPacket << endl;
    }

    EV << "Node " << nodeId
       << " summary: generated=" << generatedCount
       << ", forwarded=" << forwardedCount
       << ", received=" << receivedCount
       << ", dropNoNextHop=" << droppedNoNextHopCount
       << ", dropTtl=" << droppedTtlCount
       << ", dropDuplicate=" << droppedDuplicateCount
       << endl;
}

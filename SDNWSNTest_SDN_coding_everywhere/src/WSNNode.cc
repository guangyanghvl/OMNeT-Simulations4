#include "WSNNode.h"
#include "SDNController.h"
#include <sstream>
#include <algorithm>

Define_Module(WSNNode);

long WSNNode::totalGenerated = 0;
long WSNNode::totalForwarded = 0;
long WSNNode::totalReceived = 0;
long WSNNode::totalDroppedNoNextHop = 0;
long WSNNode::totalDroppedTtl = 0;
long WSNNode::totalDroppedDuplicate = 0;
long WSNNode::totalCodedCreated = 0;
long WSNNode::totalCodedReceived = 0;
long WSNNode::totalFlushedBuffered = 0;
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
    sinkId = par("sinkId");
    xCoord = par("xCoord").doubleValue();
    yCoord = par("yCoord").doubleValue();
    radioRange = par("radioRange").doubleValue();
    sendInterval = par("sendInterval");
    codingWaitTime = par("codingWaitTime");
    initialTtl = par("initialTtl");
    seqCounter = 0;
    sendTimer = nullptr;
    flushTimer = nullptr;

    controller = check_and_cast<SDNController *>(getParentModule()->getSubmodule("controller"));

    generatedCount = 0;
    forwardedCount = 0;
    receivedCount = 0;
    droppedNoNextHopCount = 0;
    droppedTtlCount = 0;
    droppedDuplicateCount = 0;
    codedCreatedCount = 0;
    codedReceivedCount = 0;
    flushedBufferedCount = 0;

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
        totalCodedCreated = 0;
        totalCodedReceived = 0;
        totalFlushedBuffered = 0;
        sinkHopCountSum = 0;
        sinkDelaySum = 0;

        totalNetworkEnergy = 0;
        totalNetworkGenEnergy = 0;
        totalNetworkRxEnergy = 0;
        totalNetworkTxEnergy = 0;
        totalNetworkCodingEnergy = 0;
    }

    if (!isSink) {
        sendTimer = new cMessage("sendTimer");
        flushTimer = new cMessage("flushTimer");
        scheduleAt(simTime() + uniform(0, 1), sendTimer);
    }

    EV << "Node " << nodeId
       << " x=" << xCoord
       << " y=" << yCoord
       << " sink=" << isSink << endl;
}

void WSNNode::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        if (msg == sendTimer) {
            generatePacket();
            scheduleAt(simTime() + sendInterval, sendTimer);
            return;
        }
        else if (msg == flushTimer) {
            flushOneBufferedPacket();
            scheduleFlushIfNeeded();
            return;
        }
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
        int newlyRecovered = 0;

        std::string k1 = makeOriginalKey(pkt->getSrcId(), pkt->getSeq());
        if (deliveredOriginals.find(k1) == deliveredOriginals.end()) {
            deliveredOriginals.insert(k1);
            receivedCount++;
            totalReceived++;
            newlyRecovered++;
        }

        if (pkt->isCoded()) {
            codedReceivedCount++;
            totalCodedReceived++;

            std::string k2 = makeOriginalKey(pkt->getSrcId2(), pkt->getSeq2());
            if (pkt->getSrcId2() >= 0 && deliveredOriginals.find(k2) == deliveredOriginals.end()) {
                deliveredOriginals.insert(k2);
                receivedCount++;
                totalReceived++;
                newlyRecovered++;
            }
        }

        if (newlyRecovered > 0) {
            sinkHopCountSum += pkt->getHopCount() * newlyRecovered;
            sinkDelaySum += SIMTIME_DBL(simTime() - pkt->getCreatedAt()) * newlyRecovered;
        }

        EV << "Sink received packet primary=(" << pkt->getSrcId() << "," << pkt->getSeq() << ")"
           << " coded=" << pkt->isCoded();

        if (pkt->isCoded()) {
            EV << " secondary=(" << pkt->getSrcId2() << "," << pkt->getSeq2() << ")";
        }

        EV << " newlyRecovered=" << newlyRecovered
           << " hops=" << pkt->getHopCount()
           << " delay=" << (simTime() - pkt->getCreatedAt())
           << " ttl=" << pkt->getTtl()
           << endl;

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

    tryCodingOrForward(pkt);
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
    pkt->setIsCoded(false);
    pkt->setSrcId2(-1);
    pkt->setSeq2(-1);

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

    tryCodingOrForward(pkt);
}

void WSNNode::tryCodingOrForward(WSNPacket *pkt)
{
    if (isSink || pkt->isCoded()) {
        forwardPacket(pkt);
        return;
    }

    for (auto it = codingBuffer.begin(); it != codingBuffer.end(); ++it) {
        if (canCodeWithBufferedPacket(*it, pkt)) {
            WSNPacket *codedPkt = createCodedPacket(*it, pkt);
            delete *it;
            codingBuffer.erase(it);
            delete pkt;

            if (codingBuffer.empty() && flushTimer && flushTimer->isScheduled())
                cancelEvent(flushTimer);

            forwardPacket(codedPkt);
            scheduleFlushIfNeeded();
            return;
        }
    }

    if ((int)codingBuffer.size() < 3) {
        codingBuffer.push_back(pkt);
        EV << "Node " << nodeId << " buffered packet for possible coding: "
           << pkt->getSrcId() << "-" << pkt->getSeq() << endl;
        scheduleFlushIfNeeded();
    }
    else {
        forwardPacket(pkt);
    }
}

WSNPacket *WSNNode::createCodedPacket(WSNPacket *pkt1, WSNPacket *pkt2)
{
    WSNPacket *coded = new WSNPacket("codedData");
    coded->setSrcId(pkt1->getSrcId());
    coded->setSeq(pkt1->getSeq());
    coded->setSrcId2(pkt2->getSrcId());
    coded->setSeq2(pkt2->getSeq());
    coded->setLastHopId(nodeId);
    coded->setFinalDstId(sinkId);
    coded->setHopCount(std::max(pkt1->getHopCount(), pkt2->getHopCount()));
    coded->setTtl(std::min(pkt1->getTtl(), pkt2->getTtl()));
    coded->setCreatedAt(std::min(pkt1->getCreatedAt(), pkt2->getCreatedAt()));
    coded->setIsCoded(true);

    codedCreatedCount++;
    totalCodedCreated++;

    energyCoding += codingEnergyCost;
    totalEnergy += codingEnergyCost;

    totalNetworkCodingEnergy += codingEnergyCost;
    totalNetworkEnergy += codingEnergyCost;

    EV << "Node " << nodeId << " created coded packet from ("
       << pkt1->getSrcId() << "," << pkt1->getSeq() << ") and ("
       << pkt2->getSrcId() << "," << pkt2->getSeq() << ")" << endl;

    return coded;
}

bool WSNNode::canCodeWithBufferedPacket(WSNPacket *pkt1, WSNPacket *pkt2) const
{
    if (pkt1->isCoded() || pkt2->isCoded())
        return false;
    if (pkt1->getSrcId() == pkt2->getSrcId())
        return false;
    return true;
}

void WSNNode::scheduleFlushIfNeeded()
{
    if (!flushTimer)
        return;
    if (codingBuffer.empty())
        return;
    if (!flushTimer->isScheduled())
        scheduleAt(simTime() + codingWaitTime, flushTimer);
}

void WSNNode::flushOneBufferedPacket()
{
    if (codingBuffer.empty())
        return;

    WSNPacket *pkt = codingBuffer.front();
    codingBuffer.pop_front();

    flushedBufferedCount++;
    totalFlushedBuffered++;

    EV << "Node " << nodeId << " flushing buffered uncoded packet "
       << pkt->getSrcId() << "-" << pkt->getSeq() << endl;

    forwardPacket(pkt);
}

void WSNNode::forwardPacket(WSNPacket *pkt)
{
    int nextHop = controller->getNextHop(nodeId);

    if (nextHop < 0) {
        droppedNoNextHopCount++;
        totalDroppedNoNextHop++;
        EV << "Node " << nodeId << " dropping packet: no controller route"
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
       << " coded=" << pkt->isCoded()
       << " ttl=" << pkt->getTtl()
       << " to node " << nextHop << endl;

    sendDirect(pkt, getParentModule()->getSubmodule("node", nextHop), "in");
}

std::string WSNNode::makePacketKey(WSNPacket *pkt) const
{
    std::ostringstream os;
    os << pkt->getSrcId() << "-" << pkt->getSeq();
    if (pkt->isCoded())
        os << "-coded-" << pkt->getSrcId2() << "-" << pkt->getSeq2();
    return os.str();
}

std::string WSNNode::makeOriginalKey(int srcId, int seq) const
{
    std::ostringstream os;
    os << srcId << "-" << seq;
    return os.str();
}

void WSNNode::clearCodingBuffer()
{
    while (!codingBuffer.empty()) {
        delete codingBuffer.front();
        codingBuffer.pop_front();
    }
}

void WSNNode::finish()
{
    if (sendTimer)
        cancelAndDelete(sendTimer);
    if (flushTimer) {
        if (flushTimer->isScheduled())
            cancelEvent(flushTimer);
        delete flushTimer;
    }

    clearCodingBuffer();

    recordScalar("generatedPackets", generatedCount);
    recordScalar("forwardedPackets", forwardedCount);
    recordScalar("receivedPackets", receivedCount);
    recordScalar("droppedNoNextHop", droppedNoNextHopCount);
    recordScalar("droppedTtl", droppedTtlCount);
    recordScalar("droppedDuplicate", droppedDuplicateCount);
    recordScalar("codedCreated", codedCreatedCount);
    recordScalar("codedReceived", codedReceivedCount);
    recordScalar("flushedBuffered", flushedBufferedCount);

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
        recordScalar("networkTotalCodedCreated", totalCodedCreated);
        recordScalar("networkTotalCodedReceived", totalCodedReceived);
        recordScalar("networkTotalFlushedBuffered", totalFlushedBuffered);
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
        EV << "Total coded created: " << totalCodedCreated << endl;
        EV << "Total coded received: " << totalCodedReceived << endl;
        EV << "Total flushed buffered: " << totalFlushedBuffered << endl;
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
       << ", codedCreated=" << codedCreatedCount
       << ", codedReceived=" << codedReceivedCount
       << ", flushedBuffered=" << flushedBufferedCount
       << ", dropNoNextHop=" << droppedNoNextHopCount
       << ", dropTtl=" << droppedTtlCount
       << ", dropDuplicate=" << droppedDuplicateCount
       << endl;
}

/*
 * WSNNode.h
 *
 *  Created on: Mar 25, 2026
 *      Author: guang
 */

#ifndef __WSNNODE_H_
#define __WSNNODE_H_

#include <omnetpp.h>
#include <set>
#include <string>
#include "WSNPacket_m.h"

using namespace omnetpp;

class SDNController;

class WSNNode : public cSimpleModule
{
  private:
    int nodeId;
    bool isSink;
    int sinkId;
    double xCoord;
    double yCoord;
    double radioRange;
    simtime_t sendInterval;
    int seqCounter;
    int initialTtl;
    cMessage *sendTimer;
    SDNController *controller;

    long generatedCount;
    long forwardedCount;
    long receivedCount;
    long droppedNoNextHopCount;
    long droppedTtlCount;
    long droppedDuplicateCount;

    double generationEnergyCost;
    double rxEnergyCost;
    double txEnergyCost;
    double codingEnergyCost;

    double energyGen;
    double energyRx;
    double energyTx;
    double energyCoding;
    double totalEnergy;

    std::set<std::string> seenPackets;

    static long totalGenerated;
    static long totalForwarded;
    static long totalReceived;
    static long totalDroppedNoNextHop;
    static long totalDroppedTtl;
    static long totalDroppedDuplicate;
    static long sinkHopCountSum;
    static double sinkDelaySum;

    static double totalNetworkEnergy;
    static double totalNetworkGenEnergy;
    static double totalNetworkRxEnergy;
    static double totalNetworkTxEnergy;
    static double totalNetworkCodingEnergy;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

    void generatePacket();
    void forwardPacket(WSNPacket *pkt);
    std::string makePacketKey(WSNPacket *pkt) const;
};

#endif

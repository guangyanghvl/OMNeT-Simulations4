/*
 * SDNController.h
 *
 *  Created on: Mar 25, 2026
 *      Author: guang
 */

#ifndef __SDNCONTROLLER_H_
#define __SDNCONTROLLER_H_

#include <omnetpp.h>
#include <vector>

using namespace omnetpp;

class SDNController : public cSimpleModule
{
  private:
    int numSensors;
    int numNodes;
    int sinkId;

    std::vector<int> nextHopTable;
    std::vector<int> depthToSink;
    std::vector<int> childrenCount;
    std::vector<int> subtreeSize;
    std::vector<bool> codingRelayTable;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

    void buildRoutingTable();
    void analyzeRoutingTree();
    void selectCodingRelays();
    double distanceBetween(int a, int b) const;

  public:
    int getNextHop(int nodeId) const;
    bool isCodingRelay(int nodeId) const;
};

#endif

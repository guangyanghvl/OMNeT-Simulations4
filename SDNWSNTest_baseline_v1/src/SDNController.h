/*
 * SDNController.h
 *
 *  Created on: Mar 25, 2026
 *      Author: guang
 */

#ifndef __SDNCONTROLLER_H_
#define __SDNCONTROLLER_H_

#include <omnetpp.h>

using namespace omnetpp;

class SDNController : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

#endif

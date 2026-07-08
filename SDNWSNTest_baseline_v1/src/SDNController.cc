#include "SDNController.h"

Define_Module(SDNController);

void SDNController::initialize()
{
    EV << "SDN Controller started" << endl;
}

void SDNController::handleMessage(cMessage *msg)
{
    EV << "Controller received message: " << msg->getName() << endl;
    delete msg;
}

#include "BaseStation.h"

Define_Module(BaseStation);


void BaseStation::handleMessage(cMessage *msg)
{
    delete msg;
}

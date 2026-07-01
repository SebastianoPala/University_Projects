
#ifndef __GROUP_3_PROJECT_BASESTATION_H_
#define __GROUP_3_PROJECT_BASESTATION_H_

#include <omnetpp.h>

using namespace omnetpp;

class BaseStation : public cSimpleModule
{

  protected:
    virtual void handleMessage(cMessage *msg) override;
};

#endif

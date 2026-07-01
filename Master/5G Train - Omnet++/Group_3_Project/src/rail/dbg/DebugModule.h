

#ifndef __GROUP_3_PROJECT_DEBUGMODULE_H_
#define __GROUP_3_PROJECT_DEBUGMODULE_H_

#include <omnetpp.h>

#include "../train/AccessPoint.h"
#include "../train/SimpleMobility.h"
#include "../Mapper.h"

using namespace omnetpp;

class DebugModule : public cSimpleModule
{
  protected:
    // Module Pointers
    cModule* train;
    AccessPoint* ap;
    Mapper* map;
    SimpleMobility* mobility;

    cMessage* update_values;

    double update_rate; // how fast the module updates the on-screen text

    char text[100]; // used for formatting the output

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif

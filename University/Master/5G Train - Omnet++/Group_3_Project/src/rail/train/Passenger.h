
#ifndef __GROUP_3_PROJECT_PASSENGER_H_
#define __GROUP_3_PROJECT_PASSENGER_H_

#include <omnetpp.h>

using namespace omnetpp;

class Passenger : public cSimpleModule
{
    double packet_size; // mean packet size
    double generation_delay; // mean generation delay

    cMessage * newPacketReady; // wake up message

    cModule * accessPoint;

    bool randomness; // determines if the values are random or deterministic (deterministic test)

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

#endif

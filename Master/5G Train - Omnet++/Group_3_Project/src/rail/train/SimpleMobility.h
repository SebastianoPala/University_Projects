

#ifndef __GROUP_3_PROJECT_SIMPLEMOBILITY_H_
#define __GROUP_3_PROJECT_SIMPLEMOBILITY_H_

#include <omnetpp.h>

using namespace omnetpp;

class SimpleMobility : public cSimpleModule
{
        // initial position
        double initialX;
        double initialY;

        double position; // current position

        double speed;

        double constraintAreaMinX; // starting point
        double constraintAreaMaxX; // wrap around distance
        double updateInterval; // how often the module updates the train's position

        cModule* parent; // train module pointer

        cMessage* self_move;

  protected:

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual void move();

  public:
    virtual double getTrainPos();
};

#endif

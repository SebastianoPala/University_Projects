

#include "SimpleMobility.h"

Define_Module(SimpleMobility);

void SimpleMobility::initialize()
{
    // parameters section
    initialX = par("initialX");
    initialY = par("initialY");

    speed = par("speed");

    constraintAreaMinX = par("constraintAreaMinX");
    constraintAreaMaxX = par("constraintAreaMaxX");

    updateInterval = par("updateInterval");

    parent = getParentModule();

    // graphical setup
    parent->getDisplayString().setTagArg("p",0,initialX);
    parent->getDisplayString().setTagArg("p",1,initialY);

    position = initialX;

    self_move = new cMessage("self-move");

    if(!speed){ // degeneracy test: the module turns off if speed is null
        EV << "SPEED NULL" << endl;
    }else
        scheduleAt(simTime() + updateInterval,self_move); // schedule the next movement
}

// moves the train and schedules the next movement
void SimpleMobility::handleMessage(cMessage *msg)
{
    move();
    scheduleAt(simTime() + updateInterval,msg);
}

void SimpleMobility::finish(){
    cancelAndDelete(self_move);
}

// the position is updated. If needed, performs the wraparound
void SimpleMobility::move(){
    position += speed * updateInterval;

    if(position >= constraintAreaMaxX)
        position = std::fmod(position,constraintAreaMaxX) + constraintAreaMinX; // Wrap around

    parent->getDisplayString().setTagArg("p",0,position); // graphical position
}

double SimpleMobility::getTrainPos(){
    return position;
}

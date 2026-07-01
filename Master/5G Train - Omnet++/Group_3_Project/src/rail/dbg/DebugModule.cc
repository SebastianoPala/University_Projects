#include "DebugModule.h"

Define_Module(DebugModule);

void DebugModule::initialize()
{
    if(par("debug").boolValue()){

        update_rate = par("update_rate").doubleValue();

        train = getParentModule();
        ap = check_and_cast<AccessPoint*>(train->getSubmodule("ap"));
        map = check_and_cast<Mapper*>(train->getParentModule()->getSubmodule("map"));
        mobility = check_and_cast<SimpleMobility*>(train->getSubmodule("mobility"));

        train->getDisplayString().setTagArg("t", 1, "r"); // on-screen train text setup

        update_values = new cMessage();

        scheduleAt(simTime() + update_rate, update_values);
    }
}

void DebugModule::handleMessage(cMessage *msg)
{
    sprintf(text,"Distance: %.1lf m\nRate: %lf Byte/s\nAP Queue: %i (%.0lf Bytes)",map->getDistanceFromBase(mobility->getTrainPos()),ap->currentDataRate(),ap->currentQueueLength(),ap->currentQueueBytes());

    train->getDisplayString().setTagArg("t", 0, text); // update on-screen text

    scheduleAt(simTime() + update_rate, msg);
}

void DebugModule::finish(){
    cancelAndDelete(update_values);
}

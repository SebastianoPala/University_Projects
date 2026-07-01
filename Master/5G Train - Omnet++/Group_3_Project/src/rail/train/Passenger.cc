#include "Passenger.h"

Define_Module(Passenger);

void Passenger::initialize()
{
    // parameters section
    randomness = getParentModule()->par("randomness").boolValue();

    packet_size = par("packet_size");
    generation_delay = par("generation_delay");

    newPacketReady = new cMessage("SEND NEW");

    accessPoint = getParentModule()->getSubmodule("ap");

    double delay = (randomness) ? exponential(generation_delay) : generation_delay; // the delay can be random or deterministic (deterministic test)

    scheduleAt(simTime() + delay,newPacketReady); // sends the first packet
}

void Passenger::handleMessage(cMessage *msg)
{
    cPacket * pak = new cPacket("MESS");

    double size = (randomness) ? exponential(packet_size) : packet_size; // the size can be random or deterministic (deterministic test)
    pak->setByteLength(size);
    pak->setTimestamp(simTime());

    sendDirect(pak,0,0,accessPoint,"in"); // wireless transmission

    double delay = (randomness) ? exponential(generation_delay) : generation_delay;

    scheduleAt(simTime() + delay,msg); // a new packet is sent periodically
}

void Passenger::finish(){
    cancelAndDelete(newPacketReady);
}

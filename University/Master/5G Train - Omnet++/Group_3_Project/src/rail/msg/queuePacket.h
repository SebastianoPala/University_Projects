

#ifndef RAIL_TRAIN_QUEUEPACKET_H_
#define RAIL_TRAIN_QUEUEPACKET_H_

#include <omnetpp.h>
using namespace omnetpp;


// a simple object whose purpose is to be stored in a queue taking as little space as possible
class queuePacket{
private:
    double byte_length;
    simtime_t timestamp; // the istant the stored cPacket was created

public:
    queuePacket(cPacket* pak);

    double getByteLength();
    simtime_t getTimestamp();
};



#endif /* RAIL_TRAIN_QUEUEPACKET_H_ */

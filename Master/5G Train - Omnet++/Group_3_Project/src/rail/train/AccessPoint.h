
#ifndef __TRAINTEST_ACCESSPOINT_H_
#define __TRAINTEST_ACCESSPOINT_H_

#include <omnetpp.h>
#include <queue>
#include "SimpleMobility.h"
#include "../Mapper.h"
#include "../msg/queuePacket.h"

using namespace omnetpp;

class AccessPoint : public cSimpleModule
{

    // module pointers
    SimpleMobility * mobility;
    Mapper * mapper;

    // packet queue
    std::queue<queuePacket*> queue;

    // self messages
    cPacket * eot; // end of transmission, contains the size of the transmitted packet
    cMessage * handoverCompleted;
    cMessage * send_sampling; // tells the access point when to send periodic statistics

    cModule * currentBase; // a pointer to the nearest base station

    double max_data_rate;

    double bytes_in_queue; // only for visual debugging

    double total_bytes_transmitted; // bytes transmitted after the warmup period
    double bytes_transmitted_last_interval; // bytes transmitted in the last sampling period

    double response_last_interval; // sum of packets response times in the last sampling period
    double packets_transmitted_last_interval;

    bool transmitting; // true if the access point is currently transmitting, false if after the last packet transmitted the queue is empty
                       // prevents double transmissions

    bool steady_state; // true if simulation time > warmup time

    double handover_interval; // uniform variable's width
    double handover_mean;

    bool randomness; // determines if the values are random or deterministic (deterministic test)

    double sampling_period; // how often statistics are sent

    // signals
    simsignal_t throughput;
    simsignal_t throughput_vect;
    simsignal_t response_time;
    simsignal_t response_time_avg;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;

    virtual bool checkHandover();
    virtual void attemptTransmission();

    virtual void sendStatistics();
  public:
    virtual double currentDataRate();
    virtual int currentQueueLength();
    virtual double currentQueueBytes();
};

#endif

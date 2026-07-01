#include "AccessPoint.h"

Define_Module(AccessPoint);

void AccessPoint::initialize()
{

    // signal section
    throughput = registerSignal("throughput_sig");
    throughput_vect = registerSignal("throughput_vect_sig");
    response_time = registerSignal("response_time_sig");
    response_time_avg = registerSignal("response_time_avg_sig");

    // the access point can directly access the mapper and the mobility's methods through their pointers
    mapper = check_and_cast<Mapper*>(getModuleByPath("Railway.map"));
    mobility = check_and_cast<SimpleMobility *>(getParentModule()->getSubmodule("mobility"));

    try{ // degeneracy failsafe: if no base stations are presents, getModuleByPath will crash the simulation
        currentBase = getModuleByPath("Railway.baseStations[0]");
    }catch(cRuntimeError& e){
        EV << "NO BASE STATIONS ARE PRESENT" << endl;
        currentBase = nullptr;
    }

    //parameters initialization
    randomness = getParentModule()->par("randomness").boolValue();

    bytes_in_queue = 0;
    max_data_rate = par("max_data_rate");

    handover_interval = par("handover_interval");
    handover_mean = par("handover_mean");

    sampling_period = par("sampling_period");

    // self message generation
    eot = new cPacket("Transmission completed");
    handoverCompleted = new cMessage("Handover completed");
    send_sampling = new cMessage("Send statistics");

    transmitting = false;

    // if there is no warm up period, the system will send statistics since the beginning
    steady_state = (getSimulation()->getWarmupPeriod() == 0);

    // interval related variables, for computing statistics
    total_bytes_transmitted = 0;
    bytes_transmitted_last_interval = 0;

    response_last_interval = 0;
    packets_transmitted_last_interval = 0;

    scheduleAt(simTime() + sampling_period, send_sampling); // the access point will periodically send statistics every "sampling_period" seconds
}

void AccessPoint::handleMessage(cMessage *msg)
{
    if(!steady_state)
        steady_state = simTime() > getSimulation()->getWarmupPeriod();

    // incoming passengers packets section

    if(!msg->isSelfMessage()){ // if it's a passenger's packet, the packet is added to the queue

       // a lighter custom object is created to store the information of the original message.
       queuePacket* pak = new queuePacket(check_and_cast<cPacket*>(msg));
       queue.push(pak); // the new object is stored

       bytes_in_queue += pak->getByteLength();

       delete msg; // the original message is not needed anymore
       msg = nullptr;
    }

    if(msg == send_sampling){  // periodic statistic section
        sendStatistics();
        return;
    }

    // transmission section

    bool is_self = msg && msg->isSelfMessage(); // if the ap received an eot or an handover completed
    bool is_idle = queue.size() == 1 && !transmitting; // if this condition is true, the AP was idle before receiving the current packet

    if(is_self || is_idle){ // if a transmission/handover have been completed or the AP was idle


        // if the access point completed a transmission, eot contains the information on the last transmission
        if(msg == eot && steady_state){
            total_bytes_transmitted += eot->getByteLength();
            bytes_transmitted_last_interval += eot->getByteLength();
        }

        if(queue.empty())
            transmitting = false;
        else
            attemptTransmission();
    }
}

void AccessPoint::attemptTransmission(){

    // handover section

    if(checkHandover() && handover_mean){ // if handover mean is zero, handovers are ignored

        double handover_delay = (randomness) ? uniform(handover_mean - handover_interval / 2,handover_mean + handover_interval / 2) : handover_mean;

        scheduleAt(simTime() + handover_delay, handoverCompleted); // transmission is interrupted for a random time

        return; // returns if handover occurs
    }


    // transmission section

    transmitting = true;

    queuePacket* pak = queue.front(); // packet is read from the queue
    queue.pop(); // the packet is removed from the queue

    double curr_rate = currentDataRate();

    if(!curr_rate || !currentBase){ // degeneracy case: if no transmission is possible, the access point turns itself off
        delete pak;
        return;
    }

    double transmit_time = pak->getByteLength() / curr_rate; // how long it takes to transmit the extracted packet

    bytes_in_queue -= pak->getByteLength();

    EV <<"DIMENSION: "<< pak->getByteLength() <<" -- TRANSMISSION TIME: " << transmit_time << " -- RATE: " << curr_rate << " Byte/s" <<endl; //DEBUG

    // a new packet is created using the information previously stored
    cPacket* transm_pak = new cPacket();
    transm_pak->setByteLength(pak->getByteLength());
    transm_pak->setTimestamp(pak->getTimestamp());

    // propagation delay is assumed null
    sendDirect(transm_pak,0,transmit_time,currentBase,"in");

    double response_last_packet = (simTime() + transmit_time - pak->getTimestamp()).dbl();

    EV <<"RESPONSE TIME: " << response_last_packet<<endl;

    response_last_interval += response_last_packet;
    packets_transmitted_last_interval++;

    if(steady_state){
        emit(response_time_avg,response_last_packet);
    }

    eot->setByteLength(pak->getByteLength()); // the size is saved in the eot message for the throughput statistic. Since the throughput depends on the simulation
                                              // time, the value is updated AFTER the transmission, unlike the response time

    scheduleAt(simTime() + transmit_time , eot);

    delete pak;
}

void AccessPoint::sendStatistics(){
    // the average throughput and response time of the previous interval is computed and sent
    if(steady_state){
        emit(throughput_vect, bytes_transmitted_last_interval / sampling_period);
        if(packets_transmitted_last_interval){
            emit(response_time, response_last_interval / packets_transmitted_last_interval);
        }
    }

    bytes_transmitted_last_interval = 0;
    response_last_interval = 0;
    packets_transmitted_last_interval = 0;

    scheduleAt(simTime() + sampling_period, send_sampling);
}

void AccessPoint::finish(){

    steady_state = simTime() > getSimulation()->getWarmupPeriod(); // limit case: no packets received during the simulation

    if(steady_state){

        simtime_t steady_state_interval = simTime() - getSimulation()->getWarmupPeriod();

        emit(throughput, total_bytes_transmitted / steady_state_interval);

        EV << "THROUGHPUT: " << total_bytes_transmitted / steady_state_interval << endl; // DEBUG
    }

    cancelAndDelete(eot);
    cancelAndDelete(handoverCompleted);
    cancelAndDelete(send_sampling);

    while(!queue.empty()){
        queuePacket * pak = queue.front();
        queue.pop();
        delete pak;
    }

}

// updates the base station pointer, returns true if an handover is required
bool AccessPoint::checkHandover(){

    // getNearestBase() returns null if the base station hasn't changed
    cModule * new_base = mapper->getNearestBase(mobility->getTrainPos());

    if(new_base){
        currentBase = new_base;
        return true;
    }
    return false;
}

// calculates the data rate based on the train's current position
double AccessPoint::currentDataRate(){

    double pos = mobility->getTrainPos();

    double distance = mapper->getDistanceFromBase(pos);

    return max_data_rate * exp(-pow(distance/1000,2));
}

// functions used by the debug module
int AccessPoint::currentQueueLength(){
    return queue.size();
}

double AccessPoint::currentQueueBytes(){
    return bytes_in_queue;
}


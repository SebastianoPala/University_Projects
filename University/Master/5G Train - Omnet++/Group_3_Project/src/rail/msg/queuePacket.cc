#include "queuePacket.h"

queuePacket::queuePacket(cPacket* pak){
    byte_length = pak->getByteLength();
    timestamp = pak->getTimestamp();
}

double queuePacket::getByteLength(){
    return byte_length;
}

simtime_t queuePacket::getTimestamp(){
    return timestamp;
}


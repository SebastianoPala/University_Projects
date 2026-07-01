#include "Mapper.h"

Define_Module(Mapper);

void Mapper::initialize()
{
    railway = getParentModule();

    num_base = railway->par("num_base").intValue();
    double rail_length = railway->par("rail_length").doubleValue();

    current_base = 0; // the nearest base is always 0 at the beginning

    if(!num_base)
        return; // degeneracy test

    distance = rail_length / num_base;
    starting_pos = distance/2; // the distance is computed this way in order to have 0m and 10km as the middle points between base stations, making the process
                               // of updating base stations much easier

    for(int i = 0; i < num_base; i++){ // updates the graphical position of all base stations

        cModule* base = railway->getSubmodule("baseStations",i);

        double posX = starting_pos+i*distance;
        base->getDisplayString().setTagArg("p", 0, posX);  // x

        double posY = (i&1) ? 500 : 1500;
        base->getDisplayString().setTagArg("p", 1, posY);  // y
    }
}


// calculates the nearest base station:
// returns nullptr if the nearest base station is the current one
// otherwise returns the module pointer to the new one
cModule * Mapper::getNearestBase(double trainPosX){

    int nearest_base = trainPosX / distance; // intervals are divided as: [(base_station_index)*distance , (base_station_index+1)*distance - 1]
                                             // es: base_station_index = 1 , distance = 1000 -> [1000, 1999]

    cModule * base_pointer = nullptr;

    if(current_base != nearest_base){

        current_base = nearest_base;

        base_pointer = railway->getSubmodule("baseStations",current_base);

    }

    return base_pointer;
}

// returns the distance from the current base station
double Mapper::getDistanceFromBase(double trainPosX){
    if(!num_base)
        return 0; // degeneracy case

    return fabs(starting_pos + ((current_base % num_base)  * distance) - trainPosX);
}



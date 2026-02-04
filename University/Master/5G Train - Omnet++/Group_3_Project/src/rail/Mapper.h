#ifndef __GROUP_3_PROJECT_MAPPER_H_
#define __GROUP_3_PROJECT_MAPPER_H_

#include <omnetpp.h>

using namespace omnetpp;

class Mapper : public cSimpleModule
{
  private:

    cModule* railway;

    int num_base; // number of base stations
    double distance; // distance between two consecutive base stations
    double starting_pos; // first base station offset

    int current_base; // index of the current base station

  public:

    virtual cModule* getNearestBase(double trainPosX);
    virtual double getDistanceFromBase(double trainPosX);

  protected:

    virtual void initialize() override;
};

#endif

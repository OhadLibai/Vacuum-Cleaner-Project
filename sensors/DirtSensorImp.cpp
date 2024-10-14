#include "DirtSensorImp.hpp"

#include "../common/InfraUtil.hpp"

// c'tor
DirtSensorImp::DirtSensorImp(std::shared_ptr<Space> workingspace) : workingspace(workingspace) {}

// move assignment operator
DirtSensorImp& DirtSensorImp::operator=(DirtSensorImp&& other) {
      if (this == &other)
        return *this;

    this->workingspace = other.workingspace;
    other.workingspace = nullptr;
    
    return *this;
}


int DirtSensorImp::dirtLevel() const {
    Position curr_position =  InfraUtil::SpaceUtil::getCurrLocation(*workingspace);
    return InfraUtil::SpaceUtil::getDirtLevel(*workingspace, curr_position);
}



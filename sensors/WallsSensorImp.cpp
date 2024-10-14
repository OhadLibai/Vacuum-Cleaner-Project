#include "WallsSensorImp.hpp"
#include "../common/InfraUtil.hpp"

// c'tor
WallsSensorImp::WallsSensorImp(std::shared_ptr<Space> workingspace) : workingspace(workingspace) {}

// move assignment
WallsSensor& WallsSensorImp::operator=(WallsSensorImp&& other) {
    if (this == &other)
        return *this;

    this->workingspace = other.workingspace;
    other.workingspace = nullptr;
    return *this;
}

bool WallsSensorImp::isWall(Direction d) const {
    Position curr_loc = InfraUtil::SpaceUtil::getCurrLocation(*workingspace); // DO NOT MAKE CURRENT LOCATION A REFERENCE!            
    Position new_pos = InfraUtil::PositionUtil::newSpacePosition(curr_loc, d); // passing curr_loc by value
    return InfraUtil::SpaceUtil::isObstacle(*workingspace, new_pos);
}

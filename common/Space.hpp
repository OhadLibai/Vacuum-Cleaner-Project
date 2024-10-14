#pragma once

#include <unordered_set>

#include "Position.hpp"
#include "InfraUtil.hpp"
#include <memory>


class Space {    
    
    private:

        std::unordered_set<Position> deadLocs;
        std::unordered_map<Position,int> dirts;
        Position groundZero; // aka dockingStation
        Position currLoc;
    
    public:

        /* Constructor */
        Space(std::unordered_set<Position> obstacles, Position dockingPos, std::unordered_map<Position,int> dirts);
        
        friend class InfraUtil::SpaceUtil; // utility library for using the infrastructure of Space
};
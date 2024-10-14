#include "Space.hpp"
#include <memory>

Space::Space(std::unordered_set<Position> obstacles, Position dockingPos, std::unordered_map<Position,int> dirts) : 
    deadLocs(obstacles),
    dirts(dirts),
    groundZero(dockingPos),
    currLoc(dockingPos) {}

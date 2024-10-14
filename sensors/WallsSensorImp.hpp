#pragma once

#include "WallsSensor.hpp"
#include "../common/Space.hpp"

class WallsSensorImp: public WallsSensor {
    private:
        std::shared_ptr<Space> workingspace;

    public:
        /* Constructor */
        WallsSensorImp() {};
        WallsSensorImp(std::shared_ptr<Space> workingspace);
        
        WallsSensor& operator=(WallsSensorImp&& other); // move assignment operator

	    bool isWall(Direction d) const override;
};

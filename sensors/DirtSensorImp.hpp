#pragma once

#include <unordered_map>

#include "../common/Space.hpp"
#include "DirtSensor.hpp"

class DirtSensorImp : public DirtSensor {
    private:
        std::shared_ptr<Space> workingspace;

    public:
        /* Constructor */
        DirtSensorImp() {};
        DirtSensorImp(std::shared_ptr<Space> workingspace); 

        DirtSensorImp& operator=(DirtSensorImp&& other); // move assignment operator

        int dirtLevel() const override;

};
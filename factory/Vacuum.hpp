
#pragma once

#include "CleaneryMachine.hpp"


/*  Vaccum class does not really neccessary since we dont even have a way to move (for
    instance, how Wheels are represented in the assignment? we are simulating...)
    Or even cleaning component in the settings of the assignment.
    So we just virtually update sensors and be should able command a moving steps  */

// Adding level of abstraction that goes between the vaccum cleaner and the sensors
// Adding level of abstraction in the sense that even main function does not know this class
// No one accross the code, not even TaskHandler knows about this class, except for the factory




// Imaginery class. The taskHandler does not even know the concrete object he commands!

class Vacuum : public CleaneryMachine {
    public:
        Vacuum() = default;
        ~Vacuum() = default;

        void move() override {
            // move the vaccum cleaner
        }

        void clean() override {
            // clean the vaccum cleaner
        }

        machine_type getMachineType() override {
            return machine_type::vacuum;
        }
        
};
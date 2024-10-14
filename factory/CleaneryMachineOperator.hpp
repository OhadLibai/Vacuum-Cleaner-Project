/* Pure abstract class */
#pragma once

#include "CleaneryMachine.hpp"
#include "../common/Position.hpp"

#include <memory>


/* 
We are assuming that the cleanery machine does not clean unless it gets an order to clean.
Meaning that we are not assuming the engine responsible for the cleaning component runs 
Without a stop unless he gets ordered to stop.
We are assuming that unless this component does not get oredered he does not perform a job.

Alternatively we could assume the opposite and then define an API with the cleaneryMachineOperator
That command the cleaning engine to turn on and off when i.g getting the dockingStation.
Maybe in reality the this perspective holds, but it does not effect the program anyway
Since firstly it is a simulation and secondly can be changed with nothing but a little touchups 
In the code. 

The "how" or the internal mechanism of the cleaning machine
Were not defined and essentially does not effect the virtual running. If for instance
We get demands that refer to the second predefined option, 
We could do nothing in the code that does not relate to the API of the CleaneryMachineOperator
And still get it done the same.
*/
class CleaneryMachineOperator {

    protected:

        std::shared_ptr<CleaneryMachine> machine;

    public:    

        CleaneryMachineOperator(std::shared_ptr<CleaneryMachine>& machine) : machine(machine) {}

        ~CleaneryMachineOperator() = default;
    
        virtual bool movingTo() = 0;

        virtual bool cleaningStep() = 0;
};
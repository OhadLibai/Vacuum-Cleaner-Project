
// Pure abstract class

// A small and humble definition of CleaneryMachine as an object that 
// is move and clean commandable
#pragma once

#include "../common/Position.hpp"

enum machine_type
{
    vacuum
    // more machines in the future
};

/*  Definition is hidden on purpose
    Since all is needed is communicating with 
    CleaneryMachineOperator. That class is defined 
    In order to supply with virtual representation of 
    Physical concrete CleaneryMachine.
    TaskHandler does not know how CleaneryMachine works.
    His way of passing commands to its CleaneryMachine he operates 
    Is through CleaneryMachineOperator */

/*  We does not even care how or what CleaneryMachine object
    can or can not do. We are SIMULATING and command it operate through
    our machineOperator. */ 
class CleaneryMachine {
    
    public:

        virtual void move() = 0;
        virtual void clean() = 0;
        virtual machine_type getMachineType() = 0;

};
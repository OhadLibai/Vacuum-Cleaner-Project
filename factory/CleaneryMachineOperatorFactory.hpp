/*  this factory needs to communicate with:
    cleaneryMachinFactory and of course implementing classes of 
    cleaneryMachineOperators    */

// this is 2-level factory abstraction
// first- taskSimulator does not know what machine he commands.
// second- he does not know who is the operator of the machine he got.

#pragma once

#include "CleaneryMachineOperator.hpp"
#include "CleaneryMachine.hpp"
#include <memory>


class CleaneryMachineOperatorFactory {
                                        
        private:

            CleaneryMachineOperatorFactory(); // we dont want to create an instance of this class
                                                         
        public:

            static std::unique_ptr<CleaneryMachineOperator> createOperator(std::shared_ptr<CleaneryMachine>& machine);

}; 
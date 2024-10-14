#pragma once

#include "CleaneryMachine.hpp"
#include "CleaneryMachineOperator.hpp"
#include "../common/Position.hpp"

class VacuumOperator : public CleaneryMachineOperator {
     
    public:

        VacuumOperator(std::shared_ptr<CleaneryMachine>& machine);
        ~VacuumOperator() = default;


        bool movingTo() override ;

        bool cleaningStep() override;

};
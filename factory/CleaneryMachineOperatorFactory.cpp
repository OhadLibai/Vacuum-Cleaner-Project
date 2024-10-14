#include "CleaneryMachineOperatorFactory.hpp"
#include "CleaneryMachineFactory.hpp"
#include "VacuumOperator.hpp"


CleaneryMachineOperatorFactory::CleaneryMachineOperatorFactory() {}


std::unique_ptr<CleaneryMachineOperator> CleaneryMachineOperatorFactory::createOperator(std::shared_ptr<CleaneryMachine>& machine) {
    switch (machine->getMachineType()) {
        case vacuum:
            return std::make_unique<VacuumOperator>(machine);

        default:
            return nullptr;
    }
}


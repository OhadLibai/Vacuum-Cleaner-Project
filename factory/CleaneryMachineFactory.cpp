#include "CleaneryMachineFactory.hpp"
#include "Vacuum.hpp"

CleaneryMachineFactory::CleaneryMachineFactory()
{

}

std::unique_ptr<CleaneryMachine> CleaneryMachineFactory::createMachine(machine_type this_machine) {
    switch (this_machine) {
        case vacuum:
            return std::make_unique<Vacuum>();

        default:
            return nullptr;
    }
}

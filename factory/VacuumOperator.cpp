#include "VacuumOperator.hpp"
#include "Vacuum.hpp"

VacuumOperator::VacuumOperator(std::shared_ptr<CleaneryMachine>& machine) : CleaneryMachineOperator(machine) {}


bool VacuumOperator::movingTo() { // commands moving
    machine->move();
    return true; // according to the assignment - we are allowed to presume the movement is done successfuly
} 

bool VacuumOperator::cleaningStep(){ // commands cleaning
    machine->clean();
    return true; // according to the assignment - we are allowed to presume the cleaning is done successfuly
} 


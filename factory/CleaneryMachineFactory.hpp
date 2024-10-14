#pragma once

#include "CleaneryMachine.hpp"
#include <memory>


/*  This file is CleaneryMachine factory for who wishes to perform a
    Cleaning tasks.   */


class CleaneryMachineFactory {

    public:
        CleaneryMachineFactory();
        

        static std::unique_ptr<CleaneryMachine> createMachine(machine_type this_machine = vacuum);
};    
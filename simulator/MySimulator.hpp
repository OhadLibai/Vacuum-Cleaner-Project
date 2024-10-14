#pragma once

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <unordered_set>

// infra
#include "../common/Space.hpp"
#include "../common/Logger.hpp"

// sensors
#include "../sensors/BatteryMeterImp.hpp"
#include "../sensors/DirtSensorImp.hpp"
#include "../sensors/WallsSensorImp.hpp"

// factory
#include "../factory/CleaneryMachineFactory.hpp"
#include "../factory/CleaneryMachineOperatorFactory.hpp"

// algo
#include "../algorithm/MyAlgorithm.hpp"

class MySimulator { 

   private:
      
      /*
         Members for reporting the task and following the cleaning progress
         These are essentially serves as debugging tools and for log printing
                                                                              */
      std::unordered_set<Position> FromDirtToCleanPositions; // adding here only when the spot is totally cleaned
      std::unordered_set<Position> HalfwayCleanedPostions; // places when we started to clean but did not clean them entirely      

      std::shared_ptr<Space> workingspace; // shared working space with the sensors

      size_t batCap;
      size_t maxSteps;
      size_t myScore;
      
      DirtSensorImp dirtSen;
      WallsSensorImp wallSen;
      BatteryMeterImp batMeter;

      std::string fileName; 
      std::ofstream outFile;

      std::unique_ptr<AbstractAlgorithm> algo;
      std::string algoName;
       
      std::string StepsTaken; // for the outputfile
      int cleaning_counter; // for the logger

      std::shared_ptr<CleaneryMachineOperator> machineOperator;   
      std::shared_ptr<CleaneryMachine> machine;

      bool summeryOnly;
      Logger log_file;

      size_t total_steps;
      Step algo_step;
      int DirtLeft;

      /* A word on factory:
         Simulator communicates with the operator of the machine
         provided for his mission. Assuming he does not know of the 
         "instruction set architecture" of the CleaneryMachine he command
         and still able to operate it through a machine Operator. */
         

      // about the battery   
      int updateDockingChargeFactor(int& chargeFactor);
      double chargingFormula(int chargingSteps);
      bool updateBattery(double levels);
      bool drainBattery(int units = -1);
      void chargeBattery(int units);

      // about update cleaning
      void cleaningSpotUpdate(Position this_position, int cleaning_val = -1);
      /* simulator updates by himself the dirty spots. It is reflcted inside the dirt sensor 
         do not raise an exception even if we cleaned already cleaned position. can use bool for debug purposes,
         to check whether algo decides to clean an already cleaned position */


      /* Error handlings: */
      void handleBatteryFailureError();
      void handleAlgMovingError();
      void handleCleaningError();
      void handleMachineMovingError();

      void handleMaxStepsError(); // algo exceeds its limit
      void handleFinishedOnDockError();  // algo did not finish on docking station

      /* A word on error handling:
         if the algorithm has chosen to clean an already cleaned spot
         that's not count as an error
         therfore no handling this case */


   /* Reporting stuff */
      std::string stepToString(Step step);
      
      void reportStep(Position& pos, Step step, size_t steps_num);
      
      size_t getScore(Step algo_last_step, size_t NumSteps, int DirtLeft, std::string status, bool InDock);

   public:
      /* Constructors */
      MySimulator() = default;

      void setFlags(bool summeryOnly = false);
      
      void setAlgorithm(std::unique_ptr<AbstractAlgorithm> algo, std::string algoName); // being called from main.cpp
      
      void reportEndTask();

      /* Destructor */
      // ~MySimulator() {};

      int readHouseFile(const std::string& houseFilePath); // being called from main.cpp
   //   void setAlgorithm(MyAlgorithm algo); // being called from main.cpp
      
      void setAlgorithm(std::shared_ptr<AbstractAlgorithm> algo, std::string algoName); // being called from main.cpp

      size_t getMaxSteps();

      size_t getScore() const;

      int getDirtLeft(); 

      /* Core execution  */
      void run();
   };
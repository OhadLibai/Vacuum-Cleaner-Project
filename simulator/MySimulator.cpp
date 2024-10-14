#include <filesystem>

#include "MySimulator.hpp"

// from infra
#include "../common/Parser.hpp"
#include "../common/Logger.hpp"
#include "../common/InfraUtil.hpp"

#define LOG_INFO(client, msg) log_file.log(client, LogLevel::INFO, msg)
#define LOG_WARNING(client, msg) log_file.log(client, LogLevel::WARNING, msg)
#define LOG_ERROR(client, msg) log_file.log(client, LogLevel::ERROR, msg)


// Extracts the file name without its extension and appends a new extension
std::string extractFileName(const std::string& inputFilePath) {
    
    // Get the filename without the extension
    std::filesystem::path filePath(inputFilePath);
    std::string filenameWithoutExtension = filePath.stem().string();
    
    return filenameWithoutExtension ;
}




int MySimulator::readHouseFile(const std::string& houseFilePath) {
    // get last part of the path
    std::string house = extractFileName(houseFilePath);
    this->fileName = house;

    // init log
    std::string logName = house + ".log";
    log_file.setLogFile(logName);

    // init Parser
    Parser p(log_file); 
    int houseStatus = p.init(houseFilePath);

    // check if the house file was parsed correctly
    if (houseStatus == -1) {
        LOG_ERROR(SIMULATOR, "House file was not parsed correctly");
        return -1;
    }

    // setting workingspace
    this->workingspace = std::make_shared<Space>(p.get_obstacles(), p.get_dock(), p.get_dirties()); 
    
    // set simulator fields
    this->batCap = p.get_battery_cap();
    this->maxSteps = p.get_max_step();

    // set batMet
    this->batMeter = BatteryMeterImp(this->batCap);

    // set dirtSen
    this->dirtSen = DirtSensorImp(workingspace); // move assignment

    // set wallSen
    this->wallSen = WallsSensorImp(workingspace); // move assignment

    // factory 
    this->machine = CleaneryMachineFactory::createMachine(); // the taskHandler does not know the concrete object he commands!
    this->machineOperator = CleaneryMachineOperatorFactory::createOperator(machine);

    return 0;

}   

/*--------------------------------------------------------------------------------------------------------------------------*/

/* setting algo */
void MySimulator::setAlgorithm(std::unique_ptr<AbstractAlgorithm> algo, std::string algoName) {
    this->algo = std::move(algo);
    
    /*
    this->algo.reset(nullptr);
    this->algo.swap(algo);
    */

    this->algo->setWallsSensor(wallSen);
    this->algo->setDirtSensor(dirtSen);
    this->algo->setBatteryMeter(batMeter);   
    this->algo->setMaxSteps(maxSteps);
    this->algoName = algoName;
}

/*--------------------------------------------------------------------------------------------------------------------------*/


/* 
    Clean updates
                   */

void MySimulator::cleaningSpotUpdate(Position this_position, int cleaning_val) {

    static int cleaning_counter  = 0; 

    std::pair<bool,bool> dirty_cleanedEntirely = InfraUtil::SpaceUtil::cleanSpot(*workingspace, this_position, cleaning_val);

    if (dirty_cleanedEntirely.first) {

        LOG_INFO(SIMULATOR, "Cleaning " + InfraUtil::PositionUtil::to_string(this_position));

        if (dirty_cleanedEntirely.second) {
            HalfwayCleanedPostions.erase(this_position);
            FromDirtToCleanPositions.insert(this_position);
        }

        else {
            if (!HalfwayCleanedPostions.contains(this_position))
                HalfwayCleanedPostions.insert(this_position);
        }
        
        std::string log_cleaning_count = "We performed: " + std::to_string(++cleaning_counter) + " cleaning spot steps already";
        LOG_INFO(SIMULATOR, log_cleaning_count);
    }

    else {
        // could potentially raise an error: "stayed on cleaned spot already" or something like it 
        // or write it for itself somewhere else in the code.
    }
}

/*--------------------------------------------------------------------------------------------------------------------------*/


/* 
    Charging stuff : 
                      */

int MySimulator::updateDockingChargeFactor(int& chargeFactor){
    return chargeFactor++;
}

double MySimulator::chargingFormula(int chargingSteps){
    return (chargingSteps * (((static_cast<double>(batCap)) / 20)));
}

bool MySimulator::updateBattery(double levels) { 
    int batLevel = batMeter.getBatteryState();
    batLevel += levels;
    batLevel = (batLevel > (int)batCap) ? batCap : batLevel; // batteryLevel can not be negative
    batMeter.updateBattery(batLevel);
    return (batLevel < 0 ) ? false:true; // reporting in case of exhuastive battery failure
}

bool MySimulator::drainBattery(int units) {
    return !updateBattery(units); 
}

void MySimulator::chargeBattery(int units) {
    updateBattery(units); 
}

/*--------------------------------------------------------------------------------------------------------------------------*/


/*  
    --------------
    ERROR HANDLING
    --------------
                    */


void MySimulator::handleBatteryFailureError() {
    std::string error_msg = "Battery failure error"; 

    LOG_ERROR(SIMULATOR, error_msg);
    InfraUtil::createErrorFile(fileName + "-" + algoName, error_msg);

    throw std::runtime_error(error_msg);
} 

void MySimulator::handleAlgMovingError() { // bump into wall
    std::string error_msg = "Bumped into a wall error"; 

    LOG_ERROR(SIMULATOR, error_msg);
    InfraUtil::createErrorFile(fileName + "-" + algoName, error_msg);

    throw std::runtime_error(error_msg);
} 

void MySimulator::handleCleaningError() {
    std::string error_msg = "Cleaning command machine error"; 

    LOG_ERROR(SIMULATOR, error_msg);
    InfraUtil::createErrorFile(fileName + "-" + algoName, error_msg);

    throw std::runtime_error(error_msg);
} 

void MySimulator::handleMachineMovingError() {
    std::string error_msg = "Moving command machine error";

    LOG_ERROR(SIMULATOR, "handleMachineMovingError");
    InfraUtil::createErrorFile(fileName + "-" + algoName, error_msg);

    throw std::runtime_error(error_msg);
}

void MySimulator::handleMaxStepsError() { // exceeds maxSteps limit
    std::string error_msg = "Exceeded maxSteps limit error";

    LOG_ERROR(SIMULATOR, error_msg);
    InfraUtil::createErrorFile(fileName + "-" + algoName, error_msg);

    throw std::runtime_error(error_msg);
}

void MySimulator::handleFinishedOnDockError() {
    std::string error_msg = "Finishig on dock error";

    LOG_ERROR(SIMULATOR, error_msg);
    InfraUtil::createErrorFile(fileName + "-" + algoName, error_msg);

    throw std::runtime_error(error_msg);
}

/*--------------------------------------------------------------------------------------------------------------------------*/


/*  
    ##############
    
### Core execution ###

    ##############
                      */

void MySimulator::run() {
    LOG_INFO(SIMULATOR, "\n\n --- STARTING THE SIMULATION --- \n");

    const Position base =  InfraUtil::SpaceUtil::getBase(*workingspace);  // getBase using InfraUtil
                                                                                // the program is built layers on layers.
    
    int docking_battery_factor = 0; // for battery charge factor
    total_steps = 0;

    Position curr_position = base;

    algo_step = Step::Stay;

    while ( (algo_step != Step::Finish) && (total_steps <= maxSteps) ) { // simulator does not decide by himself when we finished the mission!

        algo_step = algo->nextStep(); // the only way TaskHandler can communicate with the algorithm
        total_steps++;

        // report to log file the algo step
        reportStep(curr_position, algo_step, total_steps);
        
        /* 
            UPDATES, 
            a control block
        */

        if (algo_step == Step::Finish)
            break; // dont need to update or prepare to update anything!
        
        if (algo_step == Step::Stay) {

            if (curr_position == base) { // algo chose to charge
                updateDockingChargeFactor(docking_battery_factor); // docking_battery_factor++

                // there is no such thing "overflooded" battery charge
                // and therfore overflooded battery checking is not made.
                // we throw error only if battery is drained entirely
                double charge_units = chargingFormula(docking_battery_factor); 
                chargeBattery(charge_units);                                                               
            }


            else { // algo chose to clean curr_position
                bool cleaning_response = machineOperator->cleaningStep(); // Abstract call. Just for complementary description.
                                                                         // At the code level it does not do nothing and "blind"
                                                                         // to the program progress simulation.
                                                                         // Just decorates the program and make it more "sensible"
                if (!cleaning_response)
                    handleCleaningError();                                               

                  // can put here bool for debuging purposes
                 // bool successful_cleaning = cleaningSpotUpdate(curr_position);
                // We dont throw an error if algo chose to clean already cleaned spot!  
               cleaningSpotUpdate(curr_position);

                bool battery_exhausted = drainBattery(); // default value : -1
                if (battery_exhausted)
                    handleBatteryFailureError();
            }
        }


        else { // algo_step was to other place in space
              
            Direction algo_direction = static_cast<Direction>(algo_step); // legal cast
            
            if (wallSen.isWall(algo_direction))    // the simulator himself does not know about the concrete
                handleAlgMovingError();            // implementation or checking failures of the alg.
                                                   // therfore checks by himself if legal step was chosen

            curr_position = InfraUtil::PositionUtil::MoveInSpace(curr_position, algo_direction);

            if (curr_position == base) 
                docking_battery_factor = 0; // nullifying charging factor

            bool successful_move = machineOperator->movingTo(); // imaginery move, just for complete instructions to the "machine"
            if (!successful_move)
                handleMachineMovingError();

            bool battery_exhausted = drainBattery(); // default value : -1
            if (battery_exhausted)
                handleBatteryFailureError(); 
        }

        InfraUtil::SpaceUtil::updateCurrLocation(*workingspace, curr_position);  // crucial for the next step as it serves algo
                                                          // updated inside the singleton space throw API calls.
    }


    if (total_steps > maxSteps + 1) // algo allowed to take the last step to dock without calling finish
        handleMaxStepsError();
    
    if (!(curr_position == base))
        handleFinishedOnDockError();
        
    //moving ahead to report task
    LOG_INFO(SIMULATOR, "\n\n --- CORE EXECUTION OF THE SIMULATION IS DONE --- \n --- NOW REPORTING THE TASK --- \n");
                                            
}


/* 
    END OF SIMULATION
                       */


/*--------------------------------------------------------------------------------------------------------------------------*/

/* helper function for reporting */
std::string MySimulator::stepToString(Step step) {
    switch (step) {
        case Step::Stay:
            return "s";
        case Step::North:
            return "S"; // reverse it due to way we read the file
        case Step::South:
            return "N"; // // reverse it due to way we read the file
        case Step::East:
            return "E";
        case Step::West:
            return "W";
        case Step::Finish:
            return "F";
    }
    return "s";
}



/* for the use of the log file */
void MySimulator::reportStep(Position& pos, Step step, size_t steps_num) {

    std::string positionStr = "Current position is: " + InfraUtil::PositionUtil::to_string(pos);
    std::string stepStr = " and chosen step is: " + stepToString(step);

    StepsTaken += stepToString(step);

    // Logging the progress
    LOG_INFO(SIMULATOR, positionStr + stepStr);
    if (step != Step::Finish) // finish step does not count
        LOG_INFO(SIMULATOR, "This is step number: " + std::to_string(steps_num));
}


/*--------------------------------------------------------------------------------------------------------------------------*/


// scoring
size_t MySimulator::getScore(Step algo_last_step, size_t NumSteps, int DirtLeft, std::string status, bool InDock) {
    if (status == "DEAD")
        return maxSteps + DirtLeft*300 + 2000;
    
    else {
        if ((algo_last_step == Step::Finish) && (!InDock)) 
            return maxSteps + DirtLeft*300 + 3000;
        
        else 
            return NumSteps + DirtLeft*300 + (InDock? 0:1000);
    }
}


/* Reporting The End Of The Task */

void MySimulator::reportEndTask()  {
    Step algo_last_step= algo_step;
    size_t NumSteps = algo_last_step == Step::Finish ?  total_steps-1 :  total_steps;

    std::string finish_location = "Robot final location: ";
    Position dock = InfraUtil::SpaceUtil::getBase(*workingspace);
    Position curr_loc = InfraUtil::SpaceUtil::getCurrLocation(*workingspace);  
    bool InDock = curr_loc == dock ;
    finish_location += InDock ? "Docking station" : "Not on the docking station";

    bool battery_empty = batMeter.getBatteryState() == 0 ? true:false;
    std::unordered_set<Position> dirts = InfraUtil::SpaceUtil::DirtyPostions(*workingspace);

    std::string status;

    // figuring out the mission status with regard to battery state and remaining dirty points
    if (curr_loc != dock) {
        if (battery_empty)
            status = "DEAD";
        else 
            status = "WORKING";
    }

    else {
        if (total_steps<maxSteps && algo_last_step == Step::Finish) 
            status = "FINISHED";

        else 
            status = "WORKING";
    }
    
    std::string spots_cleaned_entirely = "Points that were cleaned entirely: \n";
    for (const Position& position : FromDirtToCleanPositions)
        spots_cleaned_entirely += InfraUtil::PositionUtil::to_string(position) + "\n";

    std::string cleaned_halfway = "Points that were cleaned but not entirely: \n";
    for (const Position& position : HalfwayCleanedPostions)
        cleaned_halfway += InfraUtil::PositionUtil::to_string(position) + "\n" ;

    std::string untouched_dirty_postions = "Dirty points we did not step on: \n" ;
    for (const Position& position:dirts){
        if (!HalfwayCleanedPostions.contains(position))
            untouched_dirty_postions += InfraUtil::PositionUtil::to_string(position) + "\n";
    }

    int DirtLeft = InfraUtil::SpaceUtil::RemainedTotalDirt(*workingspace);

    myScore = getScore(algo_last_step, NumSteps, DirtLeft,status, InDock);
  
    // writing to output file
    if (!summeryOnly) {
        std::string output = fileName + "-" + algoName + ".txt";
        outFile.open(output);
        outFile << "NumSteps = " + std::to_string(NumSteps)  << std::endl;
        outFile <<  "DirtLeft = " + std::to_string(DirtLeft) << std::endl;
        outFile << "Status = " + status << std::endl;
        outFile << "InDock = " << (InDock? "TRUE" : "FALSE") << std::endl;
        outFile << "Score = " + std::to_string(myScore) << std::endl;
        outFile << "Steps:" << std::endl << StepsTaken << std::endl;
    }

    // writing to LOG
    LOG_INFO(SIMULATOR, "Total steps: " + std::to_string(NumSteps));
    LOG_INFO(SIMULATOR, status);
    LOG_INFO(SIMULATOR, finish_location);
    LOG_INFO(SIMULATOR, "Score: " + std::to_string(myScore));
    LOG_INFO(SIMULATOR, "Total dirt left: " + std::to_string(DirtLeft));
    LOG_INFO(SIMULATOR, spots_cleaned_entirely);
    LOG_INFO(SIMULATOR, cleaned_halfway);
    LOG_INFO(SIMULATOR, untouched_dirty_postions);
    LOG_INFO(SIMULATOR, "---------- END OF REPORT ----------");
    LOG_INFO(SIMULATOR, "\n");
}

void MySimulator::setFlags(bool summeryOnly) {
    this->summeryOnly = summeryOnly;
}

size_t MySimulator::getMaxSteps() {
    return this->maxSteps;
}

size_t MySimulator::getScore() const {
    return myScore;
}

int MySimulator::getDirtLeft() {
    return InfraUtil::SpaceUtil::RemainedTotalDirt(*workingspace);
}
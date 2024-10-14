#pragma once

#include <utility>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

#include "MySimulator.hpp"
#include "../common/AlgorithmRegistrar.h"
#include "../common/Logger.hpp"

class SimulationArgs {
    public:
        std::string house_file;
        std::unique_ptr<AbstractAlgorithm> algo;
        std::string algo_name;

        SimulationArgs(const SimulationArgs&) = delete;

        // constructor with paramaters
        SimulationArgs(std::string house_file, std::unique_ptr<AbstractAlgorithm> algo_ptr, std::string algo_name) : 
        house_file(house_file), algo(std::move(algo_ptr)), algo_name(algo_name) {}

            
            // Move constructor
        SimulationArgs(SimulationArgs&& other) noexcept
            : house_file(std::move(other.house_file)),
            algo(std::move(other.algo)),
            algo_name(std::move(other.algo_name)) {}

        // Move assignment operator
        SimulationArgs& operator=(SimulationArgs&& other) noexcept {
            if (this != &other) {
                house_file = std::move(other.house_file);
                algo = std::move(other.algo);
                algo_name = std::move(other.algo_name);
            }
            return *this;
        }
};


class SimulatorManager {
public:

    SimulatorManager(const std::string& housePath, const std::string& algoPath, bool summeryOnly,  int numThreads);
    ~SimulatorManager();

    // run all simulations
    void runSimulations();
    
private:

    // void runAndTimeout(MySimulator* simulator, const SimulationArgs& task);

    // seech for .so files and open them
    void loadAlgorithms();

    void addTask(SimulationArgs&& task);

    // worker thread function
    void workerThread();

    void timeoutThread(std::future<void>& future, const SimulationArgs& task);

    // log update - thread safe
    void logUpdate(std::string msg, LogLevel level);

    // scan for all house files in the house directory
    std::vector<std::string> getAllHouses();

    // generate summary CSV file
    void generateSummaryCSV(const std::string& filename);

    // close all .so files
    void closeDll();

    std::string housePath;
    std::string algoPath;
    int numThreads;
    bool summeryOnly;
    
    std::vector<std::thread> workers; // thread pool
    std::queue<SimulationArgs> taskQueue; 
    std::mutex queueMutex;
    std::condition_variable condition; // if queue is empty or not
    
    bool stopFlag = false;
    std::vector<void*> algoPaths;
    std::vector<AlgorithmFactory> algorithms;

    // for summery csv
    std::vector<std::string> houseNames;
    std::vector<std::string> algoNames;
    std::vector<std::vector<int>> scores; // 2D vector to store scores

    Logger log_file;
    std::mutex logMutex;
   
};

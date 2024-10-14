#include "SimulatorManager.hpp"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <dlfcn.h>
#include <stdexcept>
#include <algorithm>
#include <future>
#include <thread>

namespace fs = std::filesystem;

std::string extractHouseName(const std::string& inputFilePath) {
    std::ifstream file(inputFilePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open the file: " + inputFilePath);
    }

    std::string firstLine;
    std::getline(file, firstLine);

    // remove newline character
    firstLine.erase(std::remove(firstLine.begin(), firstLine.end(), '\n'), firstLine.end());
    firstLine.erase(std::remove(firstLine.begin(), firstLine.end(), '\r'), firstLine.end());

    return firstLine;
}

void SimulatorManager::logUpdate(std::string msg, LogLevel level) {
    std::lock_guard<std::mutex> lock(logMutex);
    log_file.log(LogClient::SIMULATOR_MANAGER, level, msg);
}


SimulatorManager::SimulatorManager(const std::string& housePath, const std::string& algoPath, bool summeryOnly, int numThreads)
    : housePath(housePath), algoPath(algoPath), numThreads(numThreads), summeryOnly(summeryOnly)  {
    log_file.setLogFile("simulator_manager.log");
    loadAlgorithms();
}

SimulatorManager::~SimulatorManager() {
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    closeDll();
}


void SimulatorManager::generateSummaryCSV(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Write header
    file << "Algorithm";
    for (const auto& houseName : houseNames) {
        file << "," << extractHouseName(houseName);
    } 
    file << "\n";

    // Write rows
    for (size_t i = 0; i < algoNames.size(); ++i) {
        file << algoNames[i];
        for (size_t j = 0; j < houseNames.size(); ++j) {
            file << "," << scores[i][j];
        }
        file << "\n";
    }

    file.close();
}


void SimulatorManager::runSimulations() {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();

    auto houses = getAllHouses();

    // Initialize house and algorithm names
    houseNames = houses;    
    for (auto it = registrar.begin(); it != registrar.end(); ++it) {
        algoNames.push_back(it->name());
    }
    scores.resize(algoNames.size(), std::vector<int>(houseNames.size(), 0));

    // Start worker threads
    stopFlag = false;
    for (int i = 0; i < numThreads; ++i) {
        workers.emplace_back(&SimulatorManager::workerThread, this);
    }

    // Create pairs of houses and algorithms
    for (const auto& house : houses) {
        for (auto it = registrar.begin(); it != registrar.end(); ++it) {
            std::string algo_name = it->name();
            std::unique_ptr<AbstractAlgorithm> algo_unique_ptr = it->create();
            // AbstractAlgorithm* algo_ptr = algo_unique_ptr.get();
            // algo_unique_ptr.release();
            
            if (!(dynamic_cast<AbstractAlgorithm*>(algo_unique_ptr.get()))) { // check if valid algo
                std::string file_name = house + "-" + algo_name; 
                InfraUtil::createErrorFile(file_name, "Factory in the .so did not create a valid algorithm");
            } 

            else {
                SimulationArgs new_simulation(house, std::move(algo_unique_ptr), algo_name);
                addTask(std::move(new_simulation));
            }
        }
    }


    // Notify workers to stop and join them
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stopFlag = true;
    }
    condition.notify_all();
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    // Generate summary CSV
    generateSummaryCSV("summary.csv");
}


void SimulatorManager::loadAlgorithms() {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();

    for (const auto& entry : fs::directory_iterator(algoPath)) {
        
        if (entry.is_regular_file() && entry.path().extension() == ".so") {
            std::string file_name = entry.path().c_str();

            size_t before = registrar.count();
            auto dll = dlopen(file_name.c_str(), RTLD_LAZY);
            size_t after = registrar.count();

            if (dll && (after - before != 0))
                algoPaths.push_back(dll);

            else {
                if (!dll)
                    InfraUtil::createErrorFile(file_name,".so file could not be open");

                if (after - before == 0)
                    InfraUtil::createErrorFile(file_name, "algorithm registration failed");
            }
        }
    }
}


void SimulatorManager::addTask(SimulationArgs&& task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        taskQueue.push(std::move(task));
    }
    condition.notify_one();
}


// Helper function to get the current time as a string
std::string getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}


void SimulatorManager::workerThread() {
    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);
        condition.wait(lock, [this] { return !taskQueue.empty() || stopFlag; });

        if (stopFlag && taskQueue.empty())
            return;

        SimulationArgs task = std::move(taskQueue.front());
        taskQueue.pop();

        MySimulator simulator; // Initialize simulation
        simulator.setFlags(this->summeryOnly);
        int houseStatus = simulator.readHouseFile(task.house_file);
        if (houseStatus == -1) {
            // remove house from the list for later csv generation
            houseNames.erase(std::remove(houseNames.begin(), houseNames.end(), task.house_file), houseNames.end());
            logUpdate("Failed to read house file: " + task.house_file, LogLevel::ERROR);
            continue;
        }
        simulator.setAlgorithm(std::move(task.algo), task.algo_name);

        // Capture start time
        auto startTime = std::chrono::steady_clock::now();
        std::string startTimestamp = getCurrentTimeString();

        size_t finalScore = 0;

        // Use a lambda to wrap the member function and its arguments
        auto future = std::async(std::launch::async, [this, &simulator]() {
            return simulator.run();
        });
        
        if (future.wait_for(std::chrono::milliseconds(simulator.getMaxSteps())) == std::future_status::timeout) {
            logUpdate("Simulation for house: " + task.house_file + " timed out.", LogLevel::WARNING);
            simulator.reportEndTask(); // reporting to outputfile
            finalScore = 2000 + simulator.getMaxSteps() * 2 + simulator.getDirtLeft() * 300 ;
            // Handle timeout penalty if needed
        } else {
            future.get(); // Retrieve result or handle any exception
            simulator.reportEndTask(); // reporting to outputfile
            finalScore = simulator.getScore();
        }
        
        // Capture end time
        auto endTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = endTime - startTime;
        std::string endTimestamp = getCurrentTimeString();

        std::stringstream ss;
        ss << "Finished simulation for house: " << task.house_file
            << " with algorithm: " << task.algo_name
            << " at " << endTimestamp << std::endl;
        ss << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;
        logUpdate(ss.str(), LogLevel::INFO);

        // Update the scores vector
        auto houseIt = std::find(houseNames.begin(), houseNames.end(), task.house_file);
        auto algoIt = std::find(algoNames.begin(), algoNames.end(), task.algo_name);

        if (houseIt != houseNames.end() && algoIt != algoNames.end()) {
            int houseIndex = std::distance(houseNames.begin(), houseIt);
            int algoIndex = std::distance(algoNames.begin(), algoIt);
            scores[algoIndex][houseIndex] = (int)finalScore ;
        }

    }
}


std::vector<std::string> SimulatorManager::getAllHouses() {
    std::vector<std::string> houses;
    for (const auto& entry : fs::directory_iterator(housePath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".house") {
            houses.push_back(entry.path().string());
        }
    }
    return houses;
}


void SimulatorManager::closeDll() {
    for (auto& dll : algoPaths) {
        if (dll)
            dlclose(dll);
    }
}


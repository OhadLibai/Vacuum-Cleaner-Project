#pragma once

#include <iostream>
#include <fstream>
#include <string>

enum LogLevel { INFO, WARNING, ERROR };
enum LogClient { PARSER, SIMULATOR, ALGORITHM, SIMULATOR_MANAGER };

// singelton 
class Logger {

public:

    Logger() = default;
    ~Logger() { if (logFile.is_open()) logFile.close(); }

    void log(LogClient client, LogLevel level, const std::string& message) {
        if (logFile.is_open()) {
            logFile << toString(client, level) << message << std::endl;
        }
    }

    void setLogFile(const std::string& fileName) {
        logFile.open(fileName, std::ios::out);
    }

private:
    
    std::string toString(LogClient client, LogLevel level) {
        std::string tostring = "";

        switch (level) {
            case LogLevel::INFO: 
                tostring  += "[INFO]    ";
                break;
            case LogLevel::WARNING: 
                tostring  += "[WARNING] ";
                break;
            case LogLevel::ERROR:  
                tostring  += "[ERROR]   ";
                break;
        }

        switch (client) {
            case LogClient::PARSER: 
                tostring += "[PARSER]:    ";
                break;
            case LogClient::SIMULATOR: 
                tostring += "[SIMULATOR]: ";
                break;
            case LogClient::ALGORITHM: 
                tostring += "[ALGORITHM]: ";
                break;
            case LogClient::SIMULATOR_MANAGER: 
                tostring += "[SIMULATOR_MANAGER]: ";
                break;
        }
        return tostring;
    }

    std::ofstream logFile;
};
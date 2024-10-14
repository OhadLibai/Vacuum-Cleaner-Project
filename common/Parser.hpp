#pragma once    

#include <fstream>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>

#include "Position.hpp"
#include "Space.hpp"
#include "Logger.hpp"



class Parser {

public:

    Parser() = delete;
    Parser(Logger& log);

    int init(const std::string& file_path);

    Position& get_dock();

    std::unordered_map<Position, int>& get_dirties();

    std::unordered_set<Position>& get_obstacles();

    size_t get_max_step() const;

    size_t get_battery_cap() const;

    std::string get_house_name() const;

private:

    void parse_file(const std::string& file_path);

    void print_house_layout() const;
    
    Position dock;
    std::unordered_map<Position, int> dirties;
    std::unordered_set<Position> walls;
    size_t max_steps;
    size_t max_battery;
    std::string house_name;
    Logger& log_file;

};
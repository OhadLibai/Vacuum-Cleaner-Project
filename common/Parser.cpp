#include "Parser.hpp"
#include "Logger.hpp"
#include <sstream>


#define LOG_INFO(client, msg) log_file.log(client, LogLevel::INFO, msg)
#define LOG_WARNING(client, msg) log_file.log(client, LogLevel::WARNING, msg)
#define LOG_ERROR(client, msg) log_file.log(client, LogLevel::ERROR, msg)

Parser::Parser(Logger& log) : walls(), log_file(log) {

} 

int Parser::init(const std::string& file_path) {
    
    try {
        parse_file(file_path);
    } catch (const std::exception& e) {
        LOG_ERROR(PARSER, "Parse file in the wrong format - using default config");
        return -1;
        // max_steps = 1000;
        // max_battery = 50;
        // dock = Position(1, 1);
        // // 4 X 4 walls
        // for (int x = 0; x < 4; ++x) {
        //     for (int y = 0; y < 4; ++y) {
        //         if (x == 0 || x == 3 || y == 0 || y == 3) {
        //             walls.insert(Position(x, y));
        //         }
        //     }
        // }

        // dirties[Position(1, 2)] = 1;
        
    }
    // update log
    print_house_layout();
    return 0;

}

void Parser::parse_file(const std::string& file_path) {
    std::ifstream inFile(file_path);
    if (!inFile) {
        throw std::runtime_error("Cannot open file: " + file_path);
    }

    std::string line;
    house_name = "invalid house name";
    if (std::getline(inFile, line)) {
        house_name = line;
        LOG_INFO(PARSER,"Parsing house: " + house_name);
    }

    // Read and parse the first two lines
    if (std::getline(inFile, line)) {
        max_steps = std::stoi(line.substr(line.find("=") + 1));
    }
    if (std::getline(inFile, line)) {
        max_battery = std::stoi(line.substr(line.find("=") + 1));
    }
    
    // Read house rows and columns
    int house_rows = 0, house_cols = 0;
    if (std::getline(inFile, line)) {
        house_rows = std::stoi(line.substr(line.find("=") + 1));
    }

    if (std::getline(inFile, line)) {
        house_cols = std::stoi(line.substr(line.find("=") + 1));
    }

    // Skip any empty lines
    while (std::getline(inFile, line) && line.empty());

    int y = 0;

    std::unordered_map<Position, int> dummy_dirties;
    std::unordered_set<Position> dummy_walls;

    // Parse house layout
    while (inFile) {

        if (y >= house_rows) {
            break;
        }

        std::istringstream iss(line);
        int x = 0;
        char pos;
        while (iss.get(pos)) {

            if (x >= house_cols) {
                break;
            }

            if (pos == 'W') {
                dummy_walls.insert(Position(x, y));
            } else if (pos == 'D') {
                dock = Position(x, y);
            } else if (std::isdigit(pos)) {
                auto val = pos - '0';
                dummy_dirties[Position(x, y)] = val;
            }
            // ignoring random characters
            x++;
        }
        y++;
        if (!std::getline(inFile, line)) break;
    }

    // Add outer walls if not present 
    for (int i = -1; i < house_cols + 1; ++i) {
        dummy_walls.insert(Position(i, -1));          // Top wall
        dummy_walls.insert(Position(i, house_rows )); // Bottom wall
    }
    for (int i = -1; i < house_rows + 1; ++i) {
        dummy_walls.insert(Position(-1, i));         // Left wall
        dummy_walls.insert(Position(house_cols, i)); // Right wall
    }

    // Update the member variables
    auto dock_coor = InfraUtil::PositionUtil::get_coor(dock);
    for (const auto& dirty : dummy_dirties) {
        auto coor = InfraUtil::PositionUtil::get_coor(dirty.first);
        Position newPos (coor.first - dock_coor.first, coor.second - dock_coor.second );
        dirties[newPos] = dirty.second;
    }

    for (const auto& wall : dummy_walls) {
        auto coor = InfraUtil::PositionUtil::get_coor(wall);
        Position newPos (coor.first - dock_coor.first, coor.second - dock_coor.second );
        walls.insert(newPos);
    }

    dock = Position(0, 0);

    // print house layout
    for (int i = -1; i< house_rows + 1; ++i) {
        std::string row;
        for (int j = -1; j < house_cols + 1; ++j) {
            Position pos(j - dock_coor.first, i - dock_coor.second);
            if (pos == dock) {
                row += "D";
            } else if (dirties.find(pos) != dirties.end()) {
                row += std::to_string(dirties[pos]);
            } else if (walls.find(pos) != walls.end()) {
                row += "W";
            } else {
                row += " ";
            }
        }
        LOG_INFO(PARSER, row);
    }

}

std::string Parser::get_house_name() const {
    return house_name;
}


Position & Parser::get_dock()  {
    return dock;
}

std::unordered_map<Position, int> & Parser::get_dirties() {
    return dirties;
}

std::unordered_set<Position> & Parser::get_obstacles() {
    return walls;
}

size_t Parser::get_max_step() const {
    return max_steps;
}

size_t Parser::get_battery_cap() const {
    return max_battery;
}

void Parser::print_house_layout() const {
    // update log
    LOG_INFO(PARSER,"House layout parsed");
    LOG_INFO(PARSER,"Docking station at: " + InfraUtil::PositionUtil::to_string(dock));
    for (const auto& dirty : dirties) {
        LOG_INFO(PARSER, "Dirty at: " + InfraUtil::PositionUtil::to_string(dirty.first) + " with level: " + std::to_string(dirty.second));
    }
    for (const auto& wall : walls) {
        LOG_INFO(PARSER, "Wall at: " + InfraUtil::PositionUtil::to_string(wall));
    }
}
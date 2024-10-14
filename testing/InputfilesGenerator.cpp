// class that generates input files

#include "InputfilesGenerator.hpp"

#include <fstream>
#include <random>

#define MAX_ROWS 25
#define MAX_COLS 25

#define BATTERY_PARAMATER 2
#define MAXSTEPS_PARAMATER 1.4

#define CLEANINGNESS 0.07 
#define WALLINESS 0.07 


void InputfilesGenerator::generate(std::string input_name, std::string house_name, size_t input_num) {
    std::ofstream new_input_file(input_name + ".house");
    std::random_device rd;

    std::uniform_int_distribution row_dist(5, MAX_ROWS);
    int rows = row_dist(rd);

    std::uniform_int_distribution col_dist(5, MAX_COLS);
    int cols = col_dist(rd);
    
    std::uniform_int_distribution bat_dist((MAX_ROWS + MAX_COLS)/BATTERY_PARAMATER, BATTERY_PARAMATER*(MAX_ROWS + MAX_COLS));
    int max_battery = bat_dist(rd);

    std::uniform_int_distribution maxSteps_dist(max_battery, static_cast<int>(ceil(MAXSTEPS_PARAMATER * MAX_ROWS*MAX_COLS)));
    int max_steps = maxSteps_dist(rd);

    // 1st line
    new_input_file << std::to_string(input_num) + "-" + house_name << std::endl;

    // 2nd line
    new_input_file << "MaxSteps = " + std::to_string(max_steps) << std::endl;

    // 3rd line
    new_input_file << "MaxBattery = " + std::to_string(max_battery) << std::endl;

    // 4th line 
    new_input_file << "Rows = " + std::to_string(rows) << std::endl;

    // 5th line
    new_input_file << "Cols = " + std::to_string(cols) << std::endl;

    /* 
        generating the table 
                                */

    for (int i=0; i < cols ;i++) // first row in the house layout
        new_input_file << "W";
    new_input_file << std::endl;

    std::uniform_int_distribution dock_row_dist(2, rows-1);
    int docking_position_x = dock_row_dist(rd);

    std::uniform_int_distribution dock_col_dist(2, cols-1);
    int docking_position_y = dock_col_dist(rd);

    std::bernoulli_distribution wall_dist(WALLINESS);
    std::bernoulli_distribution clean_dist(CLEANINGNESS);
    std::uniform_int_distribution dirtiness_dist(1,9);

    for (int i=2; i <= (rows - 1); i++) {
        new_input_file << "W";

        for (int j=2; j <= (cols - 1) ; j++) {
            // Docking case
            if (i == docking_position_x && j == docking_position_y) {
                new_input_file << "D";
                continue;
            }

            bool is_wall = wall_dist(rd);

            if (is_wall) 
                new_input_file << "W";
                
            else {
                bool is_dirt = clean_dist(rd);

                if (!is_dirt)
                    new_input_file << " "; // cleaned position

                else {
                    size_t dirtiness = dirtiness_dist(rd);
                    new_input_file << std::to_string(dirtiness);
                }    
            }
        }

        new_input_file << "W" << std::endl; // end of a row so endl
    }
    
    for (int i=0; i < cols ;i++) // last row in the house layout
        new_input_file << "W";
    new_input_file << std::endl;

    // closing the file
    new_input_file.close();
}

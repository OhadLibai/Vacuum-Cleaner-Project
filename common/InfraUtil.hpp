#pragma once

#include <string>
#include <utility>
#include <unordered_map>
#include <unordered_set>

#include "Direction.hpp"

class Position; // forward declaration
class Space; // forward declaration

/*  
    This is utility lib for functionalities of some class in the program. 
*/

namespace InfraUtil {

    class PositionUtil {

        public:
        
            static int get_x(Position pos); // for logger
            static int get_y(Position pos); // for logger
            static std::pair<int,int> get_coor(Position this_position); // for the parser

            static std::string to_string(Position pos); // for logger and simulator reports

            static Position newSpacePosition(Position curr_pos, Direction Direction);  

            static Position& updatePosition(Position& curr_pos, Direction dir);    

            static Position& MoveInSpace(Position& curr_pos, Direction dir); 
                                                                   
        };


    class SpaceUtil {
        public:
            static Position getBase(Space& this_space); 

            static bool isObstacle(Space& this_space, Position this_pos);

            static Position getCurrLocation(Space& this_space);

            static void updateCurrLocation(Space& this_space, Position& curr_position); // space and curr_position have to be passed by reference
                                                                                // used by Simulator::run
                                                            
            // # DELETE getDirts afterwards                                                            
            static std::unordered_map<Position,int>& getDirts(Space& this_space);

            // first bool is whether the spot was indeed dirty. second bool is whether this spot is clean (entirely) now
            static std::pair<bool,bool> cleanSpot(Space& this_space, Position this_position, int cleaning_val = -1); 

            static int getDirtLevel(Space& this_space, Position this_position);

            static std::unordered_set<Position> DirtyPostions(Space& this_space);

            static int RemainedTotalDirt(Space& this_space);
    };
    
    // generic function for delivering error messages in error file
    void createErrorFile(std::string file_name, std::string msg); // shared between simulator and simulatorManager
}


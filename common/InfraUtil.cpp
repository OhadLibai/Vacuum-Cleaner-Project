#include "InfraUtil.hpp"
#include "Position.hpp"
#include "Space.hpp"

#include <fstream>


/* 
    PositionUtil
    ------------
*/

int InfraUtil::PositionUtil::get_x(Position pos) {
    return pos.coor.first;
}

int InfraUtil::PositionUtil::get_y(Position pos) {
    return pos.coor.second;
}

std::pair<int,int> InfraUtil::PositionUtil::get_coor(Position this_position) {
    return this_position.coor;
} 

std::string InfraUtil::PositionUtil::to_string(Position pos) {
    return "(" + std::to_string(pos.coor.first) + "," + std::to_string(pos.coor.second) + ")";
}


Position& InfraUtil::PositionUtil::updatePosition(Position& curr_pos, Direction dir) {
    switch (dir) {
        case Direction::North :
            curr_pos.coor.second++;
            break;

        case Direction::South :
            curr_pos.coor.second--;
            break;

        case Direction::East :
            curr_pos.coor.first++;
            break;

        case Direction::West :
            curr_pos.coor.first--;
            break;
        }

        return curr_pos;
}

Position InfraUtil::PositionUtil::newSpacePosition(Position curr_pos, Direction dir) {
    return InfraUtil::PositionUtil::updatePosition(curr_pos, dir);
}

Position& InfraUtil::PositionUtil::MoveInSpace(Position& curr_pos, Direction dir) {
    return InfraUtil::PositionUtil::updatePosition(curr_pos, dir);
}



/*
    SpaceUtil
    ---------
*/

Position InfraUtil::SpaceUtil::getBase(Space& this_space) {
    return this_space.groundZero;
}

bool InfraUtil::SpaceUtil::isObstacle(Space& this_space, Position this_pos) {
    auto it = this_space.deadLocs.find(this_pos);
    if (it != this_space.deadLocs.end())
        return true;

    return false;
}

Position InfraUtil::SpaceUtil::getCurrLocation(Space& this_space) {
    return this_space.currLoc;
}

void InfraUtil::SpaceUtil::updateCurrLocation(Space& this_space, Position& curr_position) {
    this_space.currLoc = curr_position;
}

std::unordered_map<Position,int>& InfraUtil::SpaceUtil::getDirts(Space& this_space) {
    return this_space.dirts;
 }


std::pair<bool,bool> InfraUtil::SpaceUtil::cleanSpot(Space& this_space, Position this_position, int cleaning_val) {

    auto it = this_space.dirts.find(this_position);
    if (it == this_space.dirts.end())
        return std::pair<bool,bool>(false, true);
    
    this_space.dirts[this_position] += cleaning_val;

    if (this_space.dirts[this_position] <= 0) {
        this_space.dirts.erase(this_position);
        return std::pair<bool,bool>(true, true);
    }

    else 
        return std::pair<bool,bool>(true, false); 
}


int InfraUtil::SpaceUtil::getDirtLevel(Space& this_space, Position this_position) {
    auto it = this_space.dirts.find(this_position);
    if (it == this_space.dirts.end())
        return 0; 
    
    return this_space.dirts[this_position];
}


std::unordered_set<Position> InfraUtil::SpaceUtil::DirtyPostions(Space& this_space) {
    std::unordered_set<Position> dirty_positions;

    for(auto& pos_dirt:this_space.dirts)
        dirty_positions.insert(pos_dirt.first);

    return dirty_positions;
}



int InfraUtil::SpaceUtil::RemainedTotalDirt(Space& this_space) {
    int cnt_dirt = 0;

    for (auto& pos_dirt:this_space.dirts) 
        cnt_dirt += pos_dirt.second;
    
    return cnt_dirt;
}


// generic function for delivering error messages in error file
void InfraUtil::createErrorFile(std::string file_name, std::string msg) {
    std::string error_filename =  file_name + "-" + ".error";
    std::ofstream error_file(error_filename);
    error_file << msg << std::endl;
    error_file.close();
}










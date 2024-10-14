#include "Position.hpp"


/* Constructors */
Position::Position() {}
Position::Position(int x, int y) : coor(x, y){}
Position::Position(std::pair<int,int> coor) : coor(coor){}

/* assignment operator */
Position& Position::operator=(const Position& other) {
    if (this == &other)
        return *this;
    this->coor = other.coor;
    return *this;
}

bool Position::operator==(const Position& other) const {
    return this->coor == other.coor;
}
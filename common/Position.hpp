#pragma once

#include <functional>  
#include <utility>
#include <string>

#include "Direction.hpp"
#include "InfraUtil.hpp"

  
class Position {

    private:
        
        std::pair<int,int> coor;

        // for hashmap uses
        struct pair_hash {
            template <class T1, class T2>
            std::size_t operator() (const std::pair<T1, T2> &pair) const {
                return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
            }
        };
        friend struct std::hash<Position>;

    public:

        /* Constructors */
        Position();
        Position(int x, int y);
        Position(std::pair<int,int> coor);
        Position(const Position&) = default;

        /* Destructor */
        ~Position() = default;

        /* assignment operator */
        Position& operator=(const Position& other); 

        bool operator==(const Position& other) const;

        friend class InfraUtil::PositionUtil; // utility library for the infra construct Position
};


// for hashmap uses
namespace std {
    template <>
    struct hash<Position> {
        size_t operator()(const Position& pos) const {
            const std::pair<int,int> & coor =  pos.coor; 
            return std::hash<int>()(coor.first) ^ (std::hash<int>()(coor.second) << 1);
        }
    };
}
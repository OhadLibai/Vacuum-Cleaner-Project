#include <mutex>

#include "algo_B.hpp"
#include "../../common/AlgorithmRegistration.h"
//register algo
REGISTER_ALGORITHM(algo_B);


MyAlgorithm::Direction_W_None algo_B::scanning_direction() {
    switch (curr_scanning_direction) {
        case MyAlgorithm::Direction_W_None::None:
            return MyAlgorithm::Direction_W_None::West;
        
        case MyAlgorithm::Direction_W_None::West:
            return MyAlgorithm::Direction_W_None::South;

        case MyAlgorithm::Direction_W_None::South:
            return MyAlgorithm::Direction_W_None::East;
        
        case MyAlgorithm::Direction_W_None::East:
            return MyAlgorithm::Direction_W_None::North;
        
        default:
            return MyAlgorithm::Direction_W_None::None;
    }
}

std::string algo_B::AlgoName() {

    // Use std::atomic for thread-safe 
    static std::mutex mtx;
    static int instance_counter = 0;
    std::lock_guard<std::mutex> lock(mtx);
    return "algo-B" + std::to_string(instance_counter++) + ".log" ;
}


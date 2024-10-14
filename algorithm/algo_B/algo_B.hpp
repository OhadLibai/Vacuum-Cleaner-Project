#include "../MyAlgorithm.hpp"

// Anti-Clockwise based

class algo_B : public MyAlgorithm {
    public:

        algo_B() = default; 
        ~algo_B() = default;
    
        
    protected:
        Direction_W_None scanning_direction() override;
        std::string AlgoName() ;
};



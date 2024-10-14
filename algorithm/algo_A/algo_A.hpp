#include "../MyAlgorithm.hpp"

// Clockwise based

class algo_A : public MyAlgorithm {

    public:
        algo_A() = default;
        ~algo_A() = default;

    protected:
        MyAlgorithm::Direction_W_None scanning_direction() override;
        std::string AlgoName() ;
};



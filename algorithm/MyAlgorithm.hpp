#include "../common/AbstractAlgorithm.hpp"
#include "../common/Logger.hpp"

#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <list>


class MyAlgorithm : public AbstractAlgorithm {

    protected:
        enum class Direction_W_None {
            North,
            East,
            South,
            West,
            None
        };

        struct BFSFLAGS {
            bool scan_visit; // true means visited in current BFS run
            Direction arrived_direction; // from which direction this Node has arrived in current BFS scan
        };

        class Node {

            public:
                std::pair<int,int> cord; // cord in space. here in order to correctly add new Nodes to our graph (all_nodes)

                std::array<std::shared_ptr<Node>, 4> neighbors;  // enter neighbors by this order: North, East, South, West   

                int dirtLevel; // born with dirtLevel = -1
                BFSFLAGS myBFSflags;
        
                Node() = delete;
                Node(int x, int y);

                bool operator==(const Node& other) const {
                         return cord == other.cord;
                }

        };

        // for hashmap uses
        struct pair_hash {
            template <class T1, class T2>
            std::size_t operator() (const std::pair<T1, T2> &pair) const {
                return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
            }
        };
        friend struct std::hash<Node>;
//------------------------------------------------------------------------------------------------------------------------------

        /* Data members of algo */

        std::unordered_map<std::pair<int,int> , std::shared_ptr<Node>,pair_hash> all_nodes;
        std::unordered_set<std::shared_ptr<Node>> NodesToHandle; // this is nodes that are KNOWN to alg but yet to be stepped, for finish feedback.

        std::pair<std::list<Direction>, std::shared_ptr<Node>> Curr_Path_Dest;

        std::shared_ptr<Node> currNode; // currNode we are stepping
        std::pair<int,int> curr_cord; // if we need to represent cords of the nodes in order to keep track in "blind space"

        std::shared_ptr<Node> DockNode; // the docking station

        Logger algoLogFile;
        Direction_W_None curr_scanning_direction; // used for the scanning routine
       
        bool DockingNavigation_flag; // false==BFS_docking constructing way to home is deactivate. true==constructing way home is active, meaning BFS_docking is activated
        bool ignore_live_bat; // flag for DockingNavigation
        bool charging; // false upon initialization
        bool include_steps_after_charging;
     
        size_t StepsForCharging;
        size_t maxBattery; // discovered by getting the bat sensor with initialization
        size_t remaining_steps; // max steps for all mission

        const BatteryMeter* batMet; 
        const WallsSensor* wallsSen; 
        const DirtSensor* dirtSen;

/*---------------------------------------------*/ 

        MyAlgorithm();
        virtual ~MyAlgorithm() = default ;

        virtual std::string AlgoName() = 0;       

        void updateCurrNode(Direction curr_dir);

        void updateCordByDir(std::pair<int,int>& cord, Direction curr_dir);



        // helper function
        Direction reversedDir(Direction dir) ;

        // simulating chargign_steps
        size_t calcStepsForCharging();

/*--------------------------------------------------------------------------------------------------------------------------*/
        /*
            scanning relavence
                                */

        void scan(std::list<std::shared_ptr<Node>>& visited, std::list<std::shared_ptr<Node>>& to_be_visited, std::shared_ptr<Node>& curr_node); // performs scanning

        std::list<Direction> extract_road(std::shared_ptr<Node>& curr_node, std::shared_ptr<Node>& origin);

        void ResetFlags(std::list<std::shared_ptr<Node>>& visited, std::list<std::shared_ptr<Node>>& to_be_visited);

        bool DockingNavigation(std::shared_ptr<Node> from, int shift = 0);

        void CleaningNavigation();


        // virtual function of the class:        
        // implemented by both algos derived classes

        virtual Direction_W_None scanning_direction() = 0;

/*--------------------------------------------------------------------------------------------------------------------------*/

        // gives what CleaningNavigation already calculated
        Direction extract_new_move();

        Step MoveToAnotherSpotRoutine();

/*--------------------------------------------------------------------------------------------------------------------------*/

        /* relates to getting on new position */

        // here we create more nodes
        void checkNewNodesRoutine();


        void checkDirtLevel();
/*--------------------------------------------------------------------------------------------------------------------------*/


/*==========================================================================================================================*/

                /* MyAlgorithm has exactly the same API as AbstractAlgorithm */
    public:
        void setMaxSteps(std::size_t maxSteps) override ;
        void setWallsSensor(const WallsSensor&) override ;
        void setDirtSensor(const DirtSensor&) override;
        void setBatteryMeter(const BatteryMeter&) override;

        Step nextStep() override;
};
/*==========================================================================================================================*/


// for hashmap uses
namespace std {
    template <>
    struct hash<MyAlgorithm::Node> {
        size_t operator()(const MyAlgorithm::Node& pos) const {
            const std::pair<int,int> & coor =  pos.cord; // make this hash function friend and let it used coor directly.
            return std::hash<int>()(coor.first) ^ (std::hash<int>()(coor.second) << 1);
        }
    };
}




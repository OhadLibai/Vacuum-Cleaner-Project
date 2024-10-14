#include "MyAlgorithm.hpp"
#include <memory>
#include <unordered_set>

#define LOG_INFO(client, msg) algoLogFile.log(client, LogLevel::INFO, msg)
#define LOG_WARNING(client, msg) algoLogFile.log(client, LogLevel::WARNING, msg)
#define LOG_ERROR(client, msg) algoLogFile.log(client, LogLevel::ERROR, msg)


MyAlgorithm::Node::Node(int x, int y) : dirtLevel(-1) {
    cord.first = x;
    cord.second = y;

    myBFSflags.scan_visit = false;
}

MyAlgorithm::MyAlgorithm() {
    
}

void MyAlgorithm::updateCurrNode(Direction curr_dir) {
    currNode = currNode->neighbors[static_cast<int>(curr_dir)];
}


void MyAlgorithm::updateCordByDir(std::pair<int,int>& cord, Direction curr_dir) {
    switch (curr_dir) {
    case Direction::North:
        cord.second++;
        break;

    case Direction::East:
        cord.first++;
        break;
    
    case Direction::South:
        cord.second--;
        break;

    case Direction::West:
        cord.first--;
        break;
    }
} 


// helper function
Direction MyAlgorithm::reversedDir(Direction dir) { 
    switch (dir) {
        case Direction::North :
            return Direction::South;

        case Direction::East :
            return Direction::West;

        case Direction::South :
            return Direction::North;

        case Direction::West :
            return Direction::East;
    }

    return Direction::North; // to avoid warning, could be printed to log...
}



/* simulating charging steps while arriving to dock */

size_t MyAlgorithm::calcStepsForCharging() { // simulating charging steps from dock when arriving with zero battery (for optimization purposes)
    double curr_bat_state = batMet->getBatteryState(); 
    size_t charging_steps = 0;

    while (static_cast<double>(maxBattery) > curr_bat_state) {
        curr_bat_state += (charging_steps * (((static_cast<double>(maxBattery)) / 20)));
        charging_steps++;
    }

    return charging_steps;
}



/*--------------------------------------------------------------------------------------------------------------------------*/

//Scanning
void MyAlgorithm::scan(std::list<std::shared_ptr<Node>>& visited, std::list<std::shared_ptr<Node>>& to_be_visited, std::shared_ptr<Node>& curr_node) {
    
    // popping the curr_node for scanning in two lines
    curr_node = to_be_visited.front();
    // curr_node = getScanNext(to_be_visited); // implementation dependent

    to_be_visited.pop_front();
    // removeScanNext(to_be_visited); // implementation dependent

    curr_node->myBFSflags.scan_visit = true;

    /* searching in curr_node neighbors */
    curr_scanning_direction = MyAlgorithm::Direction_W_None::None;
    std::shared_ptr<Node> neighbor;

    for (int i = 0 ; i < 4; i++) { // search candidates according the implementation
        curr_scanning_direction = scanning_direction(); // implementation dependent
        neighbor = curr_node->neighbors[static_cast<int>(curr_scanning_direction)];

        if (neighbor == nullptr) continue;

        if (neighbor->myBFSflags.scan_visit == true) continue; 
        
        // enter to to_be_visited means scan him later
        to_be_visited.push_back(neighbor);

        // init flags for the neighbor
        neighbor->myBFSflags.arrived_direction = static_cast<Direction>(curr_scanning_direction);
        neighbor->myBFSflags.scan_visit = true;
    }
    
    /* finished with curr_node and move it to visited */
    visited.push_back(curr_node); 
}


// Extract road after scanning
std::list<Direction> MyAlgorithm::extract_road(std::shared_ptr<Node>& curr_node, std::shared_ptr<Node>& origin) {
    std::list<Direction> path;

    while(curr_node != origin) {   // the goal is to chain a path

        Direction curr_straight_dir = curr_node->myBFSflags.arrived_direction;

        path.push_front(curr_straight_dir);

        /* find another node in the chain */
        Direction curr_reversed_dir = reversedDir(curr_straight_dir);

        curr_node = curr_node->neighbors[(int)curr_reversed_dir];

        curr_straight_dir = curr_node->myBFSflags.arrived_direction;
    }

    return path;
}


// Reset flags after scanning
void MyAlgorithm::ResetFlags(std::list<std::shared_ptr<Node>>& visited, std::list<std::shared_ptr<Node>>& to_be_visited) {
    for (auto& it : visited)
        it->myBFSflags.scan_visit = false;

    for (auto& it : to_be_visited)
        it->myBFSflags.scan_visit = false;
}

/*--------------------------------------------------------------------------------------------------------------------------*/


bool MyAlgorithm::DockingNavigation(std::shared_ptr<Node> from, int shift) {  

    /* This is BFS run to dock
    1. enforcing conditions of battery and maxSteps in regard to the docking station.
    2. in case no path is possible then create a path to dock 
    */

    std::list<std::shared_ptr<Node>> visited; // scanned entirely
    std::list<std::shared_ptr<Node>> to_be_visited; // found but not scanned

    std::shared_ptr<Node> curr_node = from;
    to_be_visited.push_back(curr_node); // initialize the scanner

    /* scanning routine*/
    do {
    scan(visited, to_be_visited, curr_node);
    } while ((curr_node != DockNode));

    /* extracting the road */
    std::list<Direction> path = extract_road(curr_node, from); // this path we are going to put in the class field with destNode
    size_t steps_to_dock = path.size();

    /* Reseting flags  */
    ResetFlags(visited, to_be_visited);


    /* ---
            checking conditions
                                    --- */

    size_t refer_to_bat_state = ignore_live_bat ? maxBattery : batMet->getBatteryState();
    size_t refer_to_remainig_steps = include_steps_after_charging ? StepsForCharging : 0;
    int remained_battery = refer_to_bat_state - shift;
    int steps_remained = remaining_steps - (shift + refer_to_remainig_steps);     

    if (DockingNavigation_flag == true) {
        if (!charging) { 
            Curr_Path_Dest.first = path; // load the path to dock    
            Curr_Path_Dest.second = DockNode;
        }

        DockingNavigation_flag = false; // cancel the flag that turned on before
        return false;
    }

    else {
        if ( (static_cast<int>(remained_battery - steps_to_dock) >= 0 ) && (static_cast<int>(steps_remained - steps_to_dock) >= 0 ) ) 
            return true; // you can go to your destination

        else {
            DockingNavigation_flag = true; // provide you with a way to go home
            return DockingNavigation(currNode);
        }
    }
}

/*--------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------*/

void MyAlgorithm::CleaningNavigation() {

    /* 
    this is version for searching new position to clean.
    makes battery checking and in case we need to go charging (or exceeding max steps)
    load a path to dock by using BFS_dock
    */

    std::list<std::shared_ptr<Node>> visited; // scanned entirely
    std::list<std::shared_ptr<Node>> to_be_visited; // found but not scanned

    std::shared_ptr<Node> curr_node = currNode;

    to_be_visited.push_back(curr_node); // initialize the scanner

    /* scannig routine */
    do {
    scan(visited, to_be_visited, curr_node);
    } while ( (!to_be_visited.empty()) && (curr_node->dirtLevel == 0) ) ;


    if (curr_node->dirtLevel == 0) { // didnt find nodes we need to go to, practically finished the task, setting you to dock
        if (currNode != DockNode) { // we get call from the dock ONLY when we fully charged
            ResetFlags(visited, to_be_visited); // because DockingNavigation is going to run now (for the last time)

            DockingNavigation_flag = true; // signal dock_navigator to set a way to home
            DockingNavigation(currNode); // DockingNavigation will construct a way home
        }

        NodesToHandle.clear();
        LOG_INFO(ALGORITHM, "Signaling to nextStep that there is no dirt points left to handle");
    }


    else {

        std::shared_ptr<Node> dest_node = curr_node; // found a new node to go
        
        std::list<Direction> path = extract_road(curr_node, currNode);
        size_t path_len = path.size();

        /* Reseting flags */
        ResetFlags(visited, to_be_visited);


        /* 
            Verifying path and acts accordingly
                                                */ 

        bool possible = DockingNavigation(dest_node, path_len + 1);

        if (possible) { // construct the path. DockingNavigation did not update the path
                        // if not: dont do anything, DockingNavigation already did it

            // load the path
            this->Curr_Path_Dest.first = path;
            this->Curr_Path_Dest.second = dest_node; 
        }

        else {
            if (currNode == DockNode) {
                LOG_INFO(ALGORITHM, "Signaling to nextStep that remaining dirts are out of reach");
                NodesToHandle.clear();
                Curr_Path_Dest.first.clear(); // override what DockNavigator did because you are sure it is not valid path
            }
        }
    }
}

/*--------------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------------*/

// gives what CleaningNavigation already calculated
Direction MyAlgorithm::extract_new_move() { 
    Direction curr_dir = Curr_Path_Dest.first.front();
    Curr_Path_Dest.first.pop_front();
    return curr_dir;
}

Step MyAlgorithm::MoveToAnotherSpotRoutine() {
    Direction curr_dir = extract_new_move(); // moving step
    updateCurrNode(curr_dir);
    updateCordByDir(curr_cord, curr_dir);
    return static_cast<Step>(curr_dir);
}

/*--------------------------------------------------------------------------------------------------------------------------*/

/* relates to getting on new position */

// here we create more nodes
void MyAlgorithm::checkNewNodesRoutine() { // the routine applies to STEPPED place in space

    if (currNode->dirtLevel == -1) {        
        Direction dirs[4] = {Direction::North, Direction::East, Direction::South, Direction::West};

        // look for your nearby friends
        for (Direction & dir: dirs) {
            if (!wallsSen->isWall(dir)) {
                
                std::pair<int,int> tmp_cord = curr_cord;
                updateCordByDir(tmp_cord,dir); // update cord in-place

                if (all_nodes.find(tmp_cord) == all_nodes.end()){ // check if this node has already discovered                                            
                    // in case this node is not exist create one
                    std::shared_ptr<Node> new_node = std::make_shared<Node>(tmp_cord.first, tmp_cord.second);

                    all_nodes[tmp_cord] = new_node;
                    NodesToHandle.insert(new_node);
                }

                // make him my friend
                currNode->neighbors[int(dir)] = all_nodes[tmp_cord];

                //do me his friend 
                all_nodes[tmp_cord]->neighbors[int(reversedDir(dir))] = currNode;
            }
        }

        if (currNode != DockNode) // DockNode is clean by definition
            currNode->dirtLevel = static_cast<int>(dirtSen->dirtLevel());
    }
}


/*--------------------------------------------------------------------------------------------------------------------------*/

void MyAlgorithm::setMaxSteps(std::size_t maxSteps)  {
    this->remaining_steps = maxSteps;
}

void MyAlgorithm::setWallsSensor(const WallsSensor& wallSens) {
    wallsSen = &wallSens;

    std::string name = AlgoName(); 
    this->algoLogFile.setLogFile(name);
    
    // parameters for initialization
    // not dependent on the order of calls of setting Sensors
    // notice that we only need the wall sensor to initialize
    // starting state (where we are in the space) 
    curr_cord = std::pair<int,int>(0,0);
    DockNode = std::make_shared<Node>(0,0);
    currNode = DockNode;
    all_nodes[curr_cord] = DockNode;

    checkNewNodesRoutine();
    DockNode->dirtLevel = 0; // set it after the routine by hand, clean by definition
}

void MyAlgorithm::setDirtSensor(const DirtSensor& dirtSens) {
    dirtSen = &dirtSens;
}

void MyAlgorithm::setBatteryMeter(const BatteryMeter& bat) {
    batMet = &bat;
    maxBattery = batMet->getBatteryState();   
}


/*--------------------------------------------------------------------------------------------------------------------------*/


/*  
    ##############

### Core execution ###

    ##############
                      */


Step MyAlgorithm::nextStep() {

    if ((!Curr_Path_Dest.first.empty()) && (!charging)) { // moving and no loaded path from dock
        remaining_steps--;
        return MoveToAnotherSpotRoutine();
    }

    else {

        if (currNode == DockNode) { // stopped on dock case //

            if (NodesToHandle.empty()) // in case you were on some location in space and discovered that: 1. there is no more steppable location in the space
                                       //                                                                 2. no remaining dirts points 
                return Step::Finish;

            if (!charging) { // new path from the dock is yet to be loaded
            /* here can add optimization for the last ride in regard to reamaining steps limit but not reccomended, probably not efficient */ 

                StepsForCharging = calcStepsForCharging();
                include_steps_after_charging = true; // dock_navigator treats as future steps
                ignore_live_bat = true; // signaling to dock_navigator that we check if we can finish before asking to charge in vain

                LOG_INFO(ALGORITHM, "At the dock, searching new spot to clean before deciding to charge");
                CleaningNavigation(); // check for new path
                
                ignore_live_bat = false;
                include_steps_after_charging = false;

                if (NodesToHandle.empty())
                    return Step::Finish;
                else {
                    LOG_INFO(ALGORITHM, "There is still nodes to handle, start charging (to full battery)"); 
                    charging = true; 
               }
            }

            if ((batMet->getBatteryState() != maxBattery)) { // charge at full before embarking to new node
                remaining_steps--;
                return Step::Stay;
            }

            else {
                charging = false; // we fully charged
                remaining_steps--;
                return MoveToAnotherSpotRoutine(); // comtinue cleaning    
            }
                
        }
        

        else { // stopped on node_to_handle position //
        
            checkNewNodesRoutine(); // essentially checks whether we should add more nodes
          
            if (currNode->dirtLevel == 0) {
                NodesToHandle.erase(currNode);

                LOG_INFO(ALGORITHM, "At (" + std::to_string(curr_cord.first) + "," + std::to_string(curr_cord.second) + ") searching for new spot to clean");
                CleaningNavigation();

                if (Curr_Path_Dest.second == DockNode)
                    LOG_INFO(ALGORITHM, "Returning to dock from: (" + std::to_string(curr_cord.first) + "," + std::to_string(curr_cord.second) + ")");
                
                remaining_steps--;
                return MoveToAnotherSpotRoutine();
            }

            else { // clean the spot, check before if battery is enough for the cleaning step
                bool enough_battery = DockingNavigation(currNode, 1); // load the way to dock if false
                LOG_INFO(ALGORITHM, "Check if we can clean spot: (" + std::to_string(curr_cord.first) + "," + std::to_string(curr_cord.second) + ") with respect to the conditions of the program");

                if (enough_battery) {
                    LOG_INFO(ALGORITHM, "Decided to clean " + std::to_string(curr_cord.first) + "," + std::to_string(curr_cord.second));

                    currNode->dirtLevel--;
                    remaining_steps--;
                    return Step::Stay;
                }
                else {
                    LOG_INFO(ALGORITHM, "Cleaning spot conditions does not satisfied therfore returning to dock from: (" + std::to_string(curr_cord.first) + "," + std::to_string(curr_cord.second) + ")");

                    remaining_steps--;
                    return MoveToAnotherSpotRoutine(); // if not enough - a path to the dock is gurenteed to wait for you there
                }
                
            }
        }
    }
}


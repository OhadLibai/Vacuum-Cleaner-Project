#include "./simulator/SimulatorManager.hpp"
#include "./testing/InputfilesGenerator.hpp"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    /*--------------------------------------------*/
    /*
    // Generate input files (for testing purposes)
    InputfilesGenerator file_generator;

    std::vector<std::string> house_names{"ohad house", "niv house", "shared condo"}; // use some house filenames
    int number_of_inputs = 3; // choose how many inputfiles you wish to create

    for (int i = 0; i < number_of_inputs; i++) 
        file_generator.generate("input_"+std::to_string(i), house_names[i], i);
    */
    /*--------------------------------------------*/
    
    // Variables to store the paths
    std::string housePath;
    std::string algoPath;
    bool summaryOnly = false;
    int numThreads = 10; // Default number of threads

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg.find("-house_path=") == 0) {
            housePath = arg.substr(std::string("-house_path=").length());
        } else if (arg.find("-algo_path=") == 0) {
            algoPath = arg.substr(std::string("-algo_path=").length());
        } else if (arg.find("-num_threads=") == 0) {
            numThreads = std::stoi(arg.substr(std::string("-num_threads=").length()));
        } else if (arg.find("-summary_only") == 0) {
            summaryOnly = true;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return EXIT_FAILURE;
        }
    }

    // Create the SimulatorManager instance
    SimulatorManager simulatorManager(housePath, algoPath, summaryOnly, numThreads);

    // Start the simulations
    simulatorManager.runSimulations();

    return 0;
}

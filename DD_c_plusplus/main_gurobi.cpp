#include <iostream>
#include <string>
#include <filesystem>

#include "Examples/GuiHandler.h"

using namespace std;
namespace fs = std::filesystem;

// Main
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "\nUsage: program_name [filename] [flags] \n";
        cout << "\tflags:\n";
        cout << "\t\t-SetCover = Problem class: Set Cover\n";
        cout << "\t\t-Knapsack = Problem class: Knapsack\n";
        cout << "\t\t-IndependentSet = Problem class: Independent Set\n";
        cout << "\t\t-SOCKnapsack = Problem class: SOC Knapsack\n";
        cout << "\t\t-Sequencing = Problem class: Sequencing\n";
        cout << "\t\t-Verbose  = Enable verbose mode\n";
        cout << "\t\t-Output_ = Output file path\n";
        cout << "\t\t-Continuous = Continuous variables; if not selected, variables will be integers\n";
        cout << "\n\n";
        return 1;
    }

    string source_directory = fs::current_path().parent_path().string();
    string filename = argv[1];
    string output_name = "";

    ProblemClass problem_class = ProblemClass::None;
    bool verbose_flag = false;
    bool continuous_flag = false;

    for (int i = 2; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-SetCover") {
            problem_class = ProblemClass::SetCover;
        } else if (arg == "-Knapsack") {
            problem_class = ProblemClass::Knapsack;
        } else if (arg == "-IndependentSet") {
            problem_class = ProblemClass::IndependentSet;
        } else if (arg == "-SOCKnapsack") {
            problem_class = ProblemClass::SOCKnapsack;
        } else if (arg == "-Sequencing") {
            problem_class = ProblemClass::Sequencing;
        } else if (arg == "-Verbose") {
            verbose_flag = true;
        } else if (arg.substr(0, 7) == "-Output") {
            output_name = arg.substr(8, arg.size());
        } else if (arg == "-Continuous") {
            continuous_flag = true;
        } 
    }

    if (problem_class == ProblemClass::None) {
        cout << "Error: A valid problem class was not specified.\n";
        return 1;
    }

    string file_path;
    if (fs::path(filename).is_absolute()) {
        file_path = filename;
    } else {
        file_path = source_directory + "/" + filename;
    }
    string output_path = "";
    if (output_name != "") {
        if (fs::path(output_name).is_absolute()) {
            output_path = output_name;
        } else {
            output_path = source_directory + "/" + output_name;
        }
    }

    // Display configuration
    cout << "Running with the following parameters:\n";
    cout << "Input File: " << filename << "\n";
    cout << "Output File: " << output_name << "\n";
    cout << "Problem Class: " << to_string(problem_class) << "\n";
    cout << "Verbose: " << (verbose_flag ? "Yes" : "No") << "\n";
    cout << "Continuous Variables: " << (continuous_flag ? "Yes" : "No") << "\n";

    // Execute the execute_dd or processing function
    GuiGurobiStructure gui_gurobi_structure = GuiGurobiStructure(file_path, output_path, problem_class, verbose_flag, continuous_flag);
    gurobi_ilp(gui_gurobi_structure);

    return 0;
}
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
        cout << "\t\t-Exact = DD Type: Exact\n";
        cout << "\t\t-Restricted = DD Type: Restricted\n";
        cout << "\t\t-RelaxPriority = DD Type: Relaxed (merge nodes by priority)\n";
        cout << "\t\t-RelaxGrouping = DD Type: Relaxed (merge nodes in groups by priority difference)\n";
        cout << "\t\t-Width_ = Maximum width for restricted/relaxed DD\n";
        cout << "\t\t-Reduce  = Enable reduction of decision diagram\n";
        cout << "\t\t-Verbose  = Enable verbose mode\n";
        cout << "\t\t-Export  = Enable export of decision diagram\n";
        cout << "\t\t-Max = Longest path; if blank, it will be the shortest path\n";
        cout << "\t\t-Min  = Shortest path\n";
        cout << "\t\t-Output_ = Output file path\n";
        cout << "\t\t-NoSort  = Disable variable ordering heuristic (default: sort enabled)\n";
        cout << "\n\n";
        return 1;
    }

    string source_directory = fs::current_path().parent_path().string();
    string filename = argv[1];
    string output_name = "";
    ProblemClass problem_class = ProblemClass::None;
    DDType dd_type = DDType::Exact;
    int max_width = 2147483646;
    bool reduce_flag = false;
    bool verbose_flag = false;
    bool export_flag = false;
    string minmax = "min";
    bool sort_flag = true;

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
        } else if (arg == "-KnapsackState") {
            problem_class = ProblemClass::KnapsackState;
        } else if (arg == "-Exact") {
            dd_type = DDType::Exact;
        } else if (arg == "-Restricted") {
            dd_type = DDType::Restricted;
        } else if (arg == "-RelaxPriority") {
            dd_type = DDType::RelaxPriority;
        } else if (arg == "-RelaxGrouping") {
            dd_type = DDType::RelaxGrouping;
        } else if (arg.substr(0, 6) == "-Width") {
            max_width = stoi(arg.substr(7));
        } else if (arg == "-Reduce") {
            reduce_flag = true;
        } else if (arg == "-Verbose") {
            verbose_flag = true;
        } else if (arg == "-Export") {
            export_flag = true;
        } else if (arg.substr(0, 7) == "-Output") {
            output_name = arg.substr(8, arg.size());
        } else if (arg == "-Max") {
            minmax = "max";
        } else if (arg == "-Min") {
            minmax = "min";
        } else if (arg == "-NoSort") {
            sort_flag = false;
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

    string output_path;
    if (!output_name.empty()) {
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
    cout << "DD Type: " << to_string(dd_type) << "\n";
    cout << "Maximum Width: " << max_width << "\n";
    cout << "Reduce: " << (reduce_flag ? "Yes" : "No") << "\n";
    cout << "Verbose: " << (verbose_flag ? "Yes" : "No") << "\n";
    cout << "Export: " << (export_flag ? "Yes" : "No") << "\n";
    cout << "Min/Max?: " << minmax << "\n";
    cout << "Sort: " << (sort_flag ? "Yes" : "No") << "\n";

    // Execute the processing function
    GuiStructure gui_structure = GuiStructure(file_path, output_path, dd_type, verbose_flag, export_flag, reduce_flag, max_width, problem_class, minmax, sort_flag);
    run_dd_code(gui_structure);
    return 0;
}

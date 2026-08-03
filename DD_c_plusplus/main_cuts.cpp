#include <iostream>
#include <string>
#include <filesystem>

#include "Examples/GuiHandler.h"

using namespace std;
namespace fs = std::filesystem;

// Main
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "\nUsage: program_name [filename] [flags]\n";
        cout << "\n\tProblem class (required):\n";
        cout << "\t\t-SetCover        Set Cover\n";
        cout << "\t\t-Knapsack        Knapsack\n";
        cout << "\t\t-IndependentSet  Independent Set\n";
        cout << "\t\t-SOCKnapsack     SOC Knapsack\n";
        cout << "\t\t-Sequencing       Sequencing\n";
        cout << "\n\tCut type (default: first valid cut for the selected problem):\n";
        cout << "\t\t-FlowCuts        Combinatorial cuts from DD\n";
        cout << "\t\t-JointFlowCuts   Dual cuts from DD\n";
        cout << "\t\t-TargetCuts      Target cuts from relaxed DD\n";
        cout << "\n\tDD type (default: Exact):\n";
        cout << "\t\t-Exact           Build the exact DD (default)\n";
        cout << "\t\t-Restricted      Build a restricted DD (requires -Width_)\n";
        cout << "\t\t-RelaxPriority   Build a relaxed DD merging by priority (requires -Width_)\n";
        cout << "\t\t-RelaxGrouping   Build a relaxed DD merging in groups by priority difference (requires -Width_)\n";
        cout << "\t\t-Width_<n>       Maximum width per layer for restricted/relaxed DD\n";
        cout << "\n\tOptions:\n";
        cout << "\t\t-Strength        Strengthen the cuts\n";
        cout << "\t\t-Verbose         Enable verbose mode\n";
        cout << "\t\t-Continuous      Solve LP relaxation (default: binary/MIP)\n";
        cout << "\t\t-Output_<name>   Output CSV file\n";
        cout << "\n\n";
        return 1;
    }

    string source_directory = fs::current_path().parent_path().string();
    string filename = argv[1];
    string output_name;

    ProblemClass problem_class  = ProblemClass::None;
    CutType      cut_type_arg   = CutType::Flow;   // placeholder; overwritten after validation
    bool         cut_explicit   = false;            // was a cut flag provided?
    bool         verbose_flag           = false;
    bool         cut_strengthening_flag = false;
    bool         continuous_flag        = false;
    bool         sort_flag              = true;
    DDType       dd_type                = DDType::Exact;
    int          max_width              = 2147483646;
    bool         width_provided         = false;

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
        } else if (arg == "-FlowCuts") {
            cut_type_arg = CutType::Flow;
            cut_explicit = true;
        } else if (arg == "-JointFlowCuts") {
            cut_type_arg = CutType::JointFlow;
            cut_explicit = true;
        } else if (arg == "-TargetCuts") {
            cut_type_arg = CutType::Target;
            cut_explicit = true;
        } else if (arg == "-Exact") {
            dd_type = DDType::Exact;
        } else if (arg == "-Restricted") {
            dd_type = DDType::Restricted;
        } else if (arg == "-RelaxPriority") {
            dd_type = DDType::RelaxPriority;
        } else if (arg == "-RelaxGrouping") {
            dd_type = DDType::RelaxGrouping;
        } else if (arg.substr(0, 7) == "-Width_") {
            max_width = stoi(arg.substr(7));
            width_provided = true;
        } else if (arg == "-Verbose") {
            verbose_flag = true;
        } else if (arg == "-CutStrengthening") {
            cut_strengthening_flag = true;
        } else if (arg == "-Continuous") {
            continuous_flag = true;
        } else if (arg.substr(0, 7) == "-Output") {
            output_name = arg.substr(8, arg.size());
        } else if (arg == "-NoSort") {
            sort_flag = false;
        }
    }

    if (problem_class == ProblemClass::None) {
        cout << "Error: A valid problem class was not specified.\n";
        return 1;
    }

    // Restricted/Relaxed DDs are width-limited and require an explicit width.
    if ((dd_type == DDType::Restricted || dd_type == DDType::RelaxPriority || dd_type == DDType::RelaxGrouping) && !width_provided) {
        cout << "Error: -" << to_string(dd_type) << " requires a width. Add -Width_<n> (e.g. -Width_4000).\n";
        return 1;
    }

    // Default to first valid cut if none was specified
    CutType cut_type = cut_explicit ? cut_type_arg : VALID_CUTS.at(problem_class).front();

    // Validate cut against the problem's supported cuts
    const auto& valid = VALID_CUTS.at(problem_class);
    if (find(valid.begin(), valid.end(), cut_type) == valid.end()) {
        cout << "Error: -" << to_string(cut_type)
             << " is not valid for " << to_string(problem_class) << ".\n";
        cout << "  Valid cuts:";
        for (CutType c : valid) cout << " -" << to_string(c);
        cout << "\n";
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
    cout << "Cut Type: " << to_string(cut_type) << "\n";
    cout << "DD Type: " << to_string(dd_type) << "\n";
    cout << "Maximum Width: " << max_width << "\n";
    cout << "Strength: " << (cut_strengthening_flag ? "Yes" : "No") << "\n";
    cout << "Verbose: " << (verbose_flag ? "Yes" : "No") << "\n";
    cout << "Continuous: " << (continuous_flag ? "Yes" : "No") << "\n";

    GuiCutsStructure gui_cuts_structure = GuiCutsStructure(
            file_path,
            output_path,
            problem_class,
            cut_type,
            cut_strengthening_flag,
            verbose_flag,
            continuous_flag,
            sort_flag,
            dd_type,
            max_width
            );
    run_gurobi_dd(gui_cuts_structure);

    return 0;
}

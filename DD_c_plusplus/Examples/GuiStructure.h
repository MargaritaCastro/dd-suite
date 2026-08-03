#ifndef DD_OPTIMIZATION_PROGRAM_GUISTRUCTURE_H
#define DD_OPTIMIZATION_PROGRAM_GUISTRUCTURE_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <tuple>
#include <map>

enum class DDType {
    None,
    Exact,
    Restricted,
    RelaxPriority,
    RelaxGrouping
};

enum class ProblemClass {
    None,
    SetCover,
    Knapsack,
    IndependentSet,
    KnapsackState,
    SOCKnapsack,
    Sequencing
};

enum class CutType {
    Flow,
    JointFlow,
    Target
};

// Cuts supported by each problem.
// BDD problems support Flow and JointFlow.
// Sequencing uses an MDD, so only TargetCuts applies.
inline const std::map<ProblemClass, std::vector<CutType>> VALID_CUTS = {
    {ProblemClass::Knapsack,       {CutType::Flow, CutType::JointFlow, CutType::Target}},
    {ProblemClass::IndependentSet, {CutType::Flow, CutType::JointFlow, CutType::Target}},
    {ProblemClass::SetCover,       {CutType::Flow, CutType::JointFlow, CutType::Target}},
    {ProblemClass::SOCKnapsack,    {CutType::Flow, CutType::JointFlow, CutType::Target}},
    {ProblemClass::Sequencing,      {CutType::Target}},
};

using namespace std;

struct GuiStructure {
    const string& input_file_path;
    const string& output_file_path = "";
    const DDType& dd_type = DDType::Exact;
    const bool& verbose = false;
    bool export_flag = false;
    bool reduce_flag = false;
    int maximum_width = 2147483646;
    const ProblemClass& problem_class = ProblemClass::Knapsack;
    const string& minmax = "min";
    bool sort_flag = true;

    explicit GuiStructure(const string& input_file_path, const string& output_file_path = "",
                 const DDType& dd_type = DDType::Exact, const bool& verbose = false, bool export_flag = false,
                 bool reduce_flag = false, int maximum_width = 2147483646, const ProblemClass& problem_class = ProblemClass::Knapsack,
                 const string& minmax = "min", bool sort_flag = true);
};

struct GuiCutsStructure {
    const string& input_file_path;
    const string& output_file_path = "";
    const ProblemClass& problem_class = ProblemClass::Knapsack;
    CutType cut_type = CutType::Flow;
    const bool& cut_strengthening_flag = false;
    const bool& continuous_flag = false;
    const bool& verbose = false;
    bool sort_flag = true;
    DDType dd_type = DDType::Exact;
    int maximum_width = 2147483646;

    explicit GuiCutsStructure(const string& input_file_path, const string& output_file_path = "",
                              const ProblemClass& problem_class = ProblemClass::Knapsack,
                              CutType cut_type = CutType::Flow,
                              const bool& cut_strengthening_flag = false, const bool& verbose = false,
                              const bool& continuous_flag = false, bool sort_flag = true,
                              DDType dd_type = DDType::Exact, int maximum_width = 2147483646);
};

struct GuiGurobiStructure {
    const string& input_file_path;
    const string& output_file_path = "";
    const ProblemClass& problem_class = ProblemClass::Knapsack;
    const bool& verbose = false;
    const bool& continuous = false;

    explicit GuiGurobiStructure(const string& input_file_path, const string& output_file_path = "",
                                const ProblemClass& problem_class = ProblemClass::Knapsack,
                                const bool& verbose = false, const bool& continuous = false);
};

#endif //DD_OPTIMIZATION_PROGRAM_GUISTRUCTURE_H

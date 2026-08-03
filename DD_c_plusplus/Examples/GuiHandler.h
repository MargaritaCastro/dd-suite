#ifndef DD_OPTIMIZATION_PROGRAM_GUIHANDLER_H
#define DD_OPTIMIZATION_PROGRAM_GUIHANDLER_H

#include "../Examples/SetCoverInstance/SetCoverProblem.h"
#include "../Examples/SetCoverInstance/SetCoverGurobiClass.h"
#include "../Examples/IndependentSetInstance/IndependentSetProblem.h"
#include "../Examples/IndependentSetInstance/IndependentSetGurobiClass.h"
#include "../Examples/KnapsackInstance/KnapsackProblem.h"
#include "../Examples/KnapsackInstance/KnapsackGurobiClass.h"
#include "../Examples/KnapsackInstance/KnapsackProblemState.h"
#include "../Examples/SetCoverInstance/SetCoverInstance.h"
#include "../Examples/IndependentSetInstance/IndependentSetInstance.h"
#include "../Examples/KnapsackInstance/KnapsackInstance.h"
#include "../Examples/SOCKnapsackInstance/SOCKnapsackInstance.h"
#include "../Examples/SOCKnapsackInstance/SOCKnapsackProblem.h"
#include "../Examples/SOCKnapsackInstance/SOCKnapsackGurobiClass.h"
#include "../Examples/SequencingInstance/SequencingInstance.h"
#include "../Examples/SequencingInstance/SequencingProblem.h"
#include "../Examples/SequencingInstance/SequencingGurobiClass.h"
#include "../Examples/SequencingInstance/SequencingPathSolver.h"
#include "DD.h"
#include "GraphAlgorithms/ShortestLongestPath/ShortestLongestPath.h"
#include "GraphAlgorithms/MaxFlow/MaxFlow.h"
#include "GraphAlgorithms/MaxFlow/MaxFlowGurobi.h"
#include "GuiStructure.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <optional>
#include <memory>
#include <chrono>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <variant>
#include <filesystem>

using namespace std;

const double tolerance = 1e-5;
/**
 * @brief Get the string representation of a ProblemClass value.
 * @param problem_class The ProblemClass value.
 * @return The string representation of the ProblemClass value.
 */
string to_string(ProblemClass problem_class);

/**
 * @brief Get the string representation of a DDType value.
 * @param dd_type The DDType value.
 * @return The string representation of the DDType value.
 */
string to_string(DDType dd_type);

/**
 * @brief Get the string representation of a CutType value.
 * @param cut_type The CutType value.
 * @return The string representation of the CutType value.
 */
string to_string(CutType cut_type);

/**
 * @brief Verify if a value is an integer considering a tolerance (tolerance).
 * @param val The value to verify.
 * @return Boolean that indicates if the value is an integer.
 */
bool is_integer(double val);

/**
 * @brief Verify if all the variables of a model are integer.
 * @param model The Gurobi model.
 * @return Boolean that indicates if all the variables are integer.
 */
bool are_all_variables_integer(const GRBModel* model);

/**
 * @brief Write statistics in an specified file.
 * @param file_path The path of the file.
 * @param statistics The statistics to write.
 * @param input_file The input file.
 */
void write_statistics(const string& file_path, const string& statistics, const string& headers);

/**
 * @brief Create and solve a decision diagram for a given problem instance.
 * @tparam T The type of the problem instance.
 * @param problem_instance The problem instance.
 * @param objective_weights The weights of the objective function.
 * @param gui_structure The GUI structure.
 */
template <typename T, typename Solver = ShortestLongestPath<T>>
void create_and_solve_dd(AbstractProblem<T>* problem_instance, const vector<double>& objective_weights, GuiStructure gui_structure);

/**
 * @brief Run the decision diagram params for a given problem instance.
 * @param gui_structure The GUI structure.
 */
void run_dd_code(GuiStructure gui_structure);

/**
 * @brief Run the Gurobi code with cutting planes for a given problem instance.
 * @tparam GurobiClass The type of the Gurobi class.
 * @tparam T The type of the state.
 * @param gui_cuts_structure The GUI cuts structure.
 * @param gurobi_instance The Gurobi class.
 * @param problem_instance The problem instance.
 */
template<typename GurobiClass, typename T>
void gurobi_dd_cutting_planes(GuiCutsStructure gui_cuts_structure, GurobiClass& gurobi_instance, AbstractProblem<T>* problem_instance);

/**
 * @brief Run the Gurobi code with DD-based cutting planes for SOC Knapsack.
 * @param gui_cuts_structure The GUI cuts structure.
 * @param gurobi_instance The SOC Knapsack Gurobi instance.
 * @param sock_params The SOC Knapsack instance parameters.
 */
void gurobi_dd_cutting_planes_soc(const GuiCutsStructure& gui_cuts_structure, SOCKnapsackGurobiClass& gurobi_instance, SOCKnapsackInstance* sock_params);

/**
 * @brief Create the instances necessary for gurobi_dd_cutting_planes planes for a given problem instance.
 * @param gui_cuts_structure The GUI cuts structure.
 **/
void run_gurobi_dd(const GuiCutsStructure& gui_cuts_structure);

/**
 * @brief Create de gurobi problem for a given problem instance.
 * @param gui_gurobi_structure The GUI Gurobi structure.
 */
void gurobi_ilp(GuiGurobiStructure gui_gurobi_structure);

#endif //DD_OPTIMIZATION_PROGRAM_GUIHANDLER_H

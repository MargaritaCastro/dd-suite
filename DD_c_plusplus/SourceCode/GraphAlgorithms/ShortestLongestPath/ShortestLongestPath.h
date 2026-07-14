#ifndef TEMPLATETYPE_OBJECTIVEFUNCTION_H
#define TEMPLATETYPE_OBJECTIVEFUNCTION_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <tuple>

#include "DD.h"
#include "GraphAlgorithms/ShortestLongestPath/PathStructure.h"

using namespace std;

/**
 * @brief Class representing an objective function controller for a decision diagram.
 * @tparam T Type of the Node state values
 */
template <typename T>
class ShortestLongestPath {
public:
    /**
     * @brief Constructor for the ShortestLongestPath class.
     * @param dd Object of the DD class containing the decision graph associated with the objective function.
     */
    ShortestLongestPath(DD<T>& dd);


    /**
     * @brief Destructor for the ShortestLongestPath class.
     */
    ~ShortestLongestPath();

    /**
     * @brief Gets the time taken to solve the shortest/longest path.
     * @return String representing the time taken.
     */
    string get_time();

    /**
     * @brief Gets the solution of the previously calculated path.
     * @return PathStructure<T> Structure containing the solution of the path.
     */
    PathStructureSolution<T> get_solution() const;

    /**
     * @brief Sets the parameters of the path weights.
     * @param weights Weights of the variables.
     * @param objective Objective to be solved.
     */
    void set_parameters(vector<double> weights, string objective="min");

    /**
     * @brief Solves the given decision diagram using the previously set weights and objective.
     * @return PathStructure<T> Structure containing the solution of the path.
     */
    PathStructureSolution<T> solve();



private:
    Graph<T>* graph; /**< Decision graph associated. */
    chrono::duration<double> time; /**< Time taken to solve the shortest/longest path. */
    PathStructureSolution<T> solution; /**< Structure storing the solution of the shortest/longest. */
    vector<double> weights; /**< Weights of the objective variables. */
    string objective; /**< The objective to be solved. */
    vector<PathStructure<T>*> DP; /**< Vector storing the results of the dynamic programming. */

    /**
 * @brief Applies necessary transformations to the weights for solving the problem if it is a maximization problem.
 */
    void choose_transform_weights();

    /**
* @brief Resolves the graph associated with the objective.
*/
    void resolve_graph();

    /**
     * @brief Saves the full path in the objective structure of the terminal node
     * @param ObjectiveStruct<T> Pointer of the terminal node to back track arc paths
     */
    void save_solution_path();

    /**
     * @brief Shortest path algorithm  in a directed acyclic graph (DAG) to solve the problem.
     * @return PathStructure<T> Pointer to the objective structure corresponding to the terminal node.
     */
    PathStructure<T>* shortest_path_algorithm();

    /**
     * @brief Reset the values of the objective nodes.
     */
    void reset_objective_nodes();

    /**
     * @brief Checks if the algorithm has been set.
     */
    void check_if_parameters_are_set();
};
#include "ShortestLongestPath.tpp"
#endif //TEMPLATETYPE_OBJECTIVEFUNCTION_H

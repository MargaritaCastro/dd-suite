#ifndef DD_OPTIMIZATION_PROGRAM_MAXFLOW_H
#define DD_OPTIMIZATION_PROGRAM_MAXFLOW_H

#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
#include <chrono>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include <cmath>
#include <deque>



using namespace std;

/**
 * @brief MaxFlow class, which is responsible for solving the maximum flow problem in a graph. With the implementation of
    Ford-Fulkerson, it seeks to find the maximum flow that can pass through a graph, from a starting node to an end node.
    This algorithm can only be used to solve binary decision diagrams (BDDs) and was specifically designed to address certain
    max-flow problems from the paper "A combinatorial cut-and-lift procedure with an application to 0–1 second-order
    conic programming. Mathematical Programming."

    Castro, M. P., Cire, A. A., & Beck, J. C. (2022). A combinatorial cut-and-lift procedure with an application to 0–1
    second-order conic programming. Mathematical Programming, 196(1), 115-171.
 * @tparam T Type of the Node state values
 */
template <typename T>
class MaxFlow {
public:
    /**
     * @brief Constructor of the MaxFlow class.
     * @param DD
     */
    MaxFlow(DD<T>* DD);
    /**
     * @brief Returns the execution time of the max-flow algorithm.
     * @return double: Time taken to solve the max-flow problem.
     */
    double get_time() const;
    /**
     * @brief Returns the precomputed values in the solve_max_flow() section.
     * @return double: Maximum flow value.
     */
    double get_max_flow() const;

    /**
     * @brief Solves the maximum flow problem in the graph.
     * @param capacities Capacities of the arc in the graph.
     * @param verbose Boolean to print additional information.
     * @return double: Maximum flow value.
     */
    double solve_max_flow(const vector<double>& capacities, bool verbose = false, const double tolerance = 1e-5);
    /**
     * @brief Returns a dictionary with the nodes that are visited in the graph.
     * @return Dictionary with the visited nodes.
     */
    vector<bool> compute_visited_nodes();
private:
    Graph<T>* graph_DD; /** Graph object that represents the DD. */
    double time; /** Execution time of the max-flow algorithm. */
    bool verbose; /** Verbose mode. */
    double max_flow; /** Maximum flow value. */
    bool max_flow_computed; /** Boolean to check if the max-flow has been computed. */
    double tolerance; /** tolerance value for the algorithm, to avoid numeric errors. */

    vector<double> residual; /** Residual capacities indexed by arc_id (even=forward, arc_id^1=reverse). */
    unordered_set<int> visited_ids; /** Visited node IDs during augmenting path search. */
    vector<Arc<T>*> arc_list; /** Arc objects indexed by arc_id/2, used for verbose printing. */

    /**
     * @brief Assigns arc_ids once at construction and pre-allocates the residual vector.
     */
    void init_arc_ids();
    /**
     * @brief Resets residual values and fills forward capacities. Arc IDs are already set.
     * @param capacities Capacities of the arc in the graph.
     */
    void set_capacities(const vector<double>& capacities);
    /**
     * @brief Computes the maximum flow in the graph.
     * @return double: Maximum flow value.
     */
    double compute_max_flow();
    /**
     * @brief Performs backtracking to find an augmenting path in the residual graph.
     * @param start_node Start node of the path.
     * @param terminal_node Terminal node of the path.
     * @param path Vector of residual arc IDs (even=forward, odd=backward).
     * @return bool: Whether a path to terminal was found.
     */
    bool find_augmenting_path(Node<T>* start_node, Node<T>* terminal_node, vector<int>& path);
    /**
     * @brief Prints the path found by the Ford-Fulkerson algorithm.
     * @param path Vector of residual arc IDs.
     * @param path_flow Flow of the path.
     */
    void print_path(const vector<int>& path, double path_flow) const;

    /**
     * @brief Resets the visited nodes in the graph dictionary.
     * @return Dictionary with the reset visited nodes.
     */
    vector<bool> reset_visited_nodes();
};


#include "MaxFlow.tpp"
#endif //DD_OPTIMIZATION_PROGRAM_MAXFLOW_H

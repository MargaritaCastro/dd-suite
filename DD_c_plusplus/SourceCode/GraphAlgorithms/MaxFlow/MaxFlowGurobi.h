#ifndef DD_OPTIMIZATION_PROGRAM_MAXFLOWGUROBI_H
#define DD_OPTIMIZATION_PROGRAM_MAXFLOWGUROBI_H

#include "MyExceptions.h"
#include "DD.h"

#include <chrono>
#include <gurobi_c++.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

/**
 * @brief MaxFlowGurobi class, which is responsible for solving the maximum flow problem in a graph. With the implementation of
    Gurobi, it seeks to find the maximum flow that can pass through a graph, from a starting node to an end node.
    This algorithm can only be used to solve binary decision diagrams (BDDs) and was specifically designed to address certain
    max-flow problems from the paper "A combinatorial cut-and-lift procedure with an application to 0–1 second-order
    conic programming. Mathematical Programming."

    Castro, M. P., Cire, A. A., & Beck, J. C. (2022). A combinatorial cut-and-lift procedure with an application to 0–1
    second-order conic programming. Mathematical Programming, 196(1), 115-171.
 * @tparam T Type of the Node state values
 */
template <typename T>
class MaxFlowGurobi {
public:
    /**
     * @brief Constructor of the MaxFlowGurobi class.
     * @param DD DD object
     */
    MaxFlowGurobi(DD<T>* DD);

    /**
    * @brief Destructor de la clase MaxFlowGurobi
    */
    ~MaxFlowGurobi();

    /**
     * @brief Method that returns the time taken to solve the problem.
     * @param double Time taken to solve the problem.
     */
    double get_time() const;
    /**
     * @brief Method that returns the maximum flow found by the algorithm.
     * @return double Maximum flow found by the algorithm.
     */
    double get_max_flow();
    /**
     * @brief Method that solves the maximum flow problem in a graph.
     * @param capacities Capacities of the arc in the graph.
     * @return double Maximum flow found by the algorithm.
     */
    double solve_max_flow(const vector<double>& capacities);

private:
    /**
     * @brief Assigns arc_ids once at construction and pre-allocates capacity/flow vectors.
     */
    void init_arc_ids();
    /**
     * @brief Fills the capacity vector indexed by arc_id >> 1.
     * @param capacities Layer capacities.
     */
    void set_capacities(const vector<double>& capacities);
    /**
     * @brief Method that computes the maximum flow in the graph.
     * @return double Maximum flow found by the algorithm.
     */
    double compute_max_flow();
    /**
     * @brief Initializes one Gurobi variable per arc, indexed by arc_id >> 1.
     */
    void initialize_variables();
    /**
     * @brief Adds flow conservation constraints in O(E) using in_arcs/out_arcs directly.
     */
    void add_constraints();
    /**
     * @brief Sets the objective to maximize outflow from source.
     */
    void set_objective();
    /**
     * @brief Method that optimizes the model.
     */
    void optimize();
    /**
     * @brief Method that shows the results of the optimization model.
     */
    void show_results() const;

    Graph<T>* graph_DD; /**< Graph of the DD object. */
    double time; /**< Time taken to solve the problem. */
    double max_flow; /**< Maximum flow found by the algorithm. */
    bool max_flow_computed; /**< Flag that indicates if the maximum flow has been computed. */

    vector<double> capacities; /**< Forward arc capacities indexed by arc_id >> 1. */
    vector<GRBVar> flows; /**< Gurobi flow variables indexed by arc_id >> 1. */
    vector<Arc<T>*> arc_list; /**< All forward arcs in traversal order. */

    GRBModel* model; /**< Optimization model. */
};

#include "MaxFlowGurobi.tpp"
#endif //DD_OPTIMIZATION_PROGRAM_MAXFLOWGUROBI_H

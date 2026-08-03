#ifndef DD_OPTIMIZATION_PROGRAM_FlowCuts_H
#define DD_OPTIMIZATION_PROGRAM_FlowCuts_H

#include <vector>
#include <deque>
#include <unordered_map>
#include <iostream>
#include <ctime>
#include <stdexcept>
#include "DD.h"
#include "GraphAlgorithms/MaxFlow/MaxFlow.h"
#include "AbstractCutGenerator.h"

using namespace std;

/**
 * @brief Cuts class, responsible for solving the cut problem in a graph. Utilizes the MaxFlow class
    to find the maximum flow and determine the minimum cuts in a given graph.
    This algorithm can only be used to solve binary decision diagrams (BDDs) and was specifically designed
    to address certain max-flow problems from the paper "A combinatorial cut-and-lift procedure with an
    application to 0–1 second-order conic programming. Mathematical Programming, section 5.3."

    Castro, M. P., Cire, A. A., & Beck, J. C. (2022). A combinatorial cut-and-lift procedure with an
    application to 0–1 second-order conic programming. Mathematical Programming, 196(1), 115-171.
 * @tparam T Type of the Node state values
 */
template <typename T>
class FlowCuts : public AbstractCutGenerator<T> {
public:
    /**
     * @brief Constructor for the Combinatorial Cuts class.
     * @param DD Decision diagram.
     */
    FlowCuts(DD<T>* DD);

    /**
     * @brief Destroyer of the FlowCuts class
     */
    ~FlowCuts();

    /**
     * @brief Returns the execution time of the cut algorithm.
     * @return Execution time of the cut algorithm.
     */
    double get_time() override;

    /**
 * @brief Returns the name of the cut generator.
 * @return String representation of the cut generator.
 */
    string get_name() override;


    /**
     * @brief Returns the precomputed values from the generate_cut() method,
     * this cut can be used to take out some options of the optimization problem.
     * @return Tuple with the found cut and the value of the cut.
     * The cut is represented by a vector of coefficients and a constant.
     * As the form of the cut is: coefficients * x <= constant.
     */
    pair<vector<double>, double> get_cut() override;

    /**
     * @brief Returns the precomputed value of the min cut
     * @return Minimum cut value
     */
    double get_min_cut();

    /**
     * @brief Solves the decision diagram, finding a cut.
     * @param x_values Values of the variables.
     * @param verbose Boolean to print additional information.
     * @return Boolean indicating whether the solution was successfully found.
     */
    bool generate_cut(vector<double>& x_values, bool verbose = false) override;

private:
    Graph<T>* graph_DD; /** Graph representation of the decision diagram */
    MaxFlow<T>* max_flow_instance; /** Instance of the MaxFlow class */
    double time; /** Time taken to solve the cut problem */
    bool verbose; /** Boolean to print additional information */
    double tolerance; /** Tolerance value for the cut, to avoid numeric errors */
    vector<double> x_values; /** Values of the variables */
    double min_cut; /** Minimum cut value */

    /**
     * @brief Computes the cut for the given x_values.
     * @param x_values Values of the variables.
     * @return Boolean indicating whether the cut was successfully computed.
     */
    bool compute_cut(const vector<double>& x_values);
    /**
     * @brief Separates the minimum cut in the graph.
     * @return Boolean indicating whether the minimum cut was successfully separated.
     */
    bool separate_min_cut();
    /**
     * @brief Gets the minimum cut arc in the graph.
     */
    void get_min_cut_arcs();

    /**
     * @brief Returns the arc from the key.
     * @return Boolean indicating whether the constraint is violated
     */
    bool is_constraint_violated() const;
};


#include "FlowCuts.tpp"
#endif //DD_OPTIMIZATION_PROGRAM_FlowCuts_H

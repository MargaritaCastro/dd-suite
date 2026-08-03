#ifndef DD_OPTIMIZATION_PROGRAM_STRENGTHCUT_H
#define DD_OPTIMIZATION_PROGRAM_STRENGTHCUT_H

#include <vector>
#include <unordered_map>
#include <string>
#include <cmath>
#include <ctime>
#include <iostream>
#include "Graph.h"

using namespace std;

/**
 * @brief CutStrengthening class, responsible for strengthening cuts in a graph.
 * @tparam T Type of the Node state values
 */
template <typename T>
class CutStrengthening {
public:
    /**
     * @brief Constructor for the CutStrengthening class.
     * @param DD Decision diagram.
     */
    CutStrengthening(DD<T>* DD, bool verbose = false);

    /**
     * @brief Returns the execution time of the strengthening algorithm.
     * @return double: Time taken to solve the strengthening cut problem.
     */
    double get_time() const;

    /**
     * @brief Returns the precomputed values from the cut_strengthening() method,
     * this cut can be used to take out some options of the optimization problem.
     * @return Tuple with the strengthened cut and its value.
     * The cut is represented by a vector of coefficients and a constant.
     * As the form of the cut is: coefficients * x <= constant.
     */
    pair<vector<double>, double> get_cut_strengthening();

    /**
     * @brief Strengthens the cut by improving the coefficients and right-hand side value.
     * @param coefficients Coefficients of the cut.
     * @param constant Constant of the cut.
     * @return bool: if the cut is strengthened
     */
    bool cut_strengthening(const vector<double>& coefficients, double constant);

private:
    /**
     * @brief Initializes the cost dictionaries for top and bottom costs.
     */
    void set_initial_cost();
    /**
     * @brief Initializes the parameters for the strengthening algorithm.
     * @param coefficients Coefficients of the cut.
     * @param constant Constant of the cut.
     */
    void initialize_parameters(const vector<double>& coefficients, double constant);
    /**
     * @brief Computes the strengthening cut algorithm.
     * @return bool: if the cut is strengthened
     */
    bool compute_cut_strengthening();
    /**
     * @brief Computes the slacks in the BDD structure.
     * @return bool: if the slacks are computed
     */
    bool get_bdd_slacks();
    /**
     * @brief Computes the top costs for each node in the graph.
     * @return bool: if the top costs are computed
     */
    void compute_cost_top();
    /**
     * @brief Computes the top costs for a given node in the graph.
     * @param node Node to compute the top cost.
     * @param layer_index Index of the layer where the node is located.
     */
    void compute_cost_top_node(Node<T>* node, size_t layer_index);
    /**
     * @brief Computes the bottom costs for each node in the graph.
     */
    void compute_cost_bottom();
    /**
     * @brief Computes the bottom costs for a given node in the graph.
     * @param node Node to compute the bottom cost.
     * @param layer_index Index of the layer where the node is located.
     */
    void compute_cost_bottom_node(Node<T>* node, size_t layer_index);
    /**
     * @brief Computes the slacks for each layer  in the graph.
     */
    void compute_slacks();
    /**
     * @brief Chooses which slack to strengthen based on the smallest absolute value greater than tolerance.
     * @return int: Index of the slack to strengthen.
     */
    int choose_slack_to_strengthen();
    /**
     * @brief Returns the inequality representing the strengthened cut.
     * @return string: Inequality representing the strengthened cut.
     */
    string get_inequality_string_representation() const;

    Graph<T>* graph_DD; /** Graph representation of the decision diagram */
    vector<double> coefficients; /** Coefficients of the cut */
    double constant; /** Constant of the cut */
    vector<double> slacks; /** Slacks of the BDD */
    vector<double> cost_top; /** Top costs of the nodes */
    vector<double> cost_bottom; /** Bottom costs of the nodes */
    double time; /** Time taken to solve the strengthening cut problem */
    bool verbose; /** Boolean to print additional information */
    double tolerance; /** Tolerance value for the cut, to avoid numeric errors */
    bool is_cut_strengthened; /** Boolean to check if the cut is strengthened */
};

#include "CutStrengthening.tpp"
#endif //DD_OPTIMIZATION_PROGRAM_STRENGTHCUT_H

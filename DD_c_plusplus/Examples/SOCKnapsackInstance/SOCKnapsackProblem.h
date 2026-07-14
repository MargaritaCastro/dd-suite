#ifndef DD_OPTIMIZATION_PROGRAM_SOCKNAPSACKPROBLEM_H
#define DD_OPTIMIZATION_PROGRAM_SOCKNAPSACKPROBLEM_H

#include "AbstractProblemClass.h"
#include "MyExceptions.h"
#include "SOCKnapsackInstance.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <memory>
#include <limits>
#include <numeric>

using namespace std;

/**
 * @brief Class representing a Second-Order Cone (SOC) Knapsack Problem.
 *
 * The feasibility constraint is:
 *   state[0] + omega * sqrt(state[1]) <= capacity
 *
 * The state is a vector<double> of size 2:
 *   [0] = accumulated linear weight sum
 *   [1] = accumulated weight sum inside the square-root term (squared)
 *
 * Special sentinel states:
 *   state[0] == -1.0  →  only zero-arcs are feasible from this node onward
 *   state[0] == -2.0  →  all arcs are always feasible from this node onward
 */
class SOCKnapsackProblem : public AbstractProblem<vector<double>> {
public:
    /**
     * @brief Constructor for the SOCKnapsackProblem class.
     *
     * Prunes zero-weight variables, then (if sort=true) sorts by decreasing
     * (linear + omega*sqrt) weight and renames surviving variables
     * lexicographically (x_1, x_2, ...). Precomputes pruning bounds.
     *
     * @param initial_state  Initial state: {0.0, 0.0}.
     * @param variables      Original variable names and binary domains.
     * @param linear_weights Linear weight of each variable in the constraint.
     * @param sqrt_weights   Weight of each variable inside the square-root term.
     * @param omega          Scaling factor for the square-root term.
     * @param capacity       Right-hand side of the SOC constraint.
     * @param sort           If true, apply sort_variables() ordering heuristic.
     */
    SOCKnapsackProblem(vector<double>* initial_state,
                       const SOCKnapsackInstance& params,
                       int constraint_index,
                       bool sort = true);

    /** Maps original variable name → its 0-based index in the Gurobi model's x vector. */
    map<string, int> original_name_to_index;

    /** Pruned variable list keeping original names (before renaming), in original order. */
    vector<pair<string, vector<int>>> original_variable_names_domains;

    mutable int count_1 = 0; /**< Number of nodes pruned to only-zero-arcs sentinel. */
    mutable int count_2 = 0; /**< Number of nodes pruned to always-feasible sentinel. */

    // ---- AbstractProblem interface ----

    /**
     * @brief Sort pruned variables by decreasing (linear + omega*sqrt) weight,
     *        rename them lexicographically (x_1, x_2, ...), and build
     *        dd_to_original_index. Also reorders linear_weights, sqrt_weights,
     *        and original_variable_names_domains to match the new order.
     */
    vector<string> sort_variables(const vector<pair<string, vector<int>>>& variables) override;

    bool transition_function(const vector<double>* previous_state,
                             int variable_index,
                             int variable_value,
                             vector<double>* scratch_state) const override;

    int get_priority_for_discard_node(const vector<double>* state) const override;
    int get_priority_for_merge_nodes(int node_id, const vector<double>* state) const override;
    vector<double>* merge_operator(const vector<double>* state_one, const vector<double>* state_two) const override;
    string get_state_as_string(const vector<double>* state) const override;
    vector<double>* get_state_copy(const vector<double>* state) const override;
    vector<double>* get_final_state() override;

private:
    // ---- Problem parameters (post-prune, post-sort) ----
    vector<int> linear_weights;   /**< Linear weights for surviving variables (sorted order). */
    vector<int> sqrt_weights;     /**< Sqrt weights for surviving variables (sorted order). */
    vector<int> sqrt_weights_2;   /**< Precomputed squares of sqrt_weights. */
    int omega;
    int capacity;

    // ---- Intermediate pruned weights (used by sort_variables) ----
    vector<int> _pruned_linear_weights; /**< Pruned linear weights in original variable order. */
    vector<int> _pruned_sqrt_weights;   /**< Pruned sqrt weights in original variable order. */

    // ---- Precomputed pruning bounds ----
    vector<int> min_linear_weight; /**< min(linear_weights[i+1:]) for each i. */
    vector<int> min_sqrt_weight;   /**< min(sqrt_weights_2[i+1:]) for each i. */
    vector<int> sum_linear_weight; /**< sum(linear_weights[i+1:]) for each i. */
    vector<int> sum_sqrt_weight;   /**< sum(sqrt_weights_2[i+1:]) for each i. */

    // ---- Construction helpers ----

    /** Internal struct holding pruned (but not yet sorted) data. */
    struct PrunedData {
        vector<pair<string, vector<int>>> pruned_variables;  /**< Pruned vars, original names. */
        vector<int> pruned_linear_weights;
        vector<int> pruned_sqrt_weights;
        map<string, int> original_name_to_index;
    };

    /** Delegating constructor that receives already-pruned data. */
    SOCKnapsackProblem(vector<double>* initial_state,
                       PrunedData&& pd,
                       int omega,
                       int capacity,
                       bool sort);

    /**
     * @brief Remove variables with both weights == 0 and build original_name_to_index.
     *        Does not sort or rename variables.
     */
    static PrunedData _prune(const vector<pair<string, vector<int>>>& variables,
                              const vector<int>& linear_weights,
                              const vector<int>& sqrt_weights);

    /** Precompute min/sum suffix arrays for fast pruning in transition_function. */
    void _compute_min_and_sum_for_pruning();

    /** Returns true if only zero-arcs can be feasible in all remaining layers. */
    bool _is_only_zero_arcs_feasible_in_next_layers(const vector<double>* state,
                                                     int variable_index) const;

    /** Returns true if one-arcs are always feasible in all remaining layers. */
    bool _is_one_arc_always_feasible_in_next_layers(const vector<double>* state,
                                                     int variable_index) const;
};

#endif //DD_OPTIMIZATION_PROGRAM_SOCKNAPSACKPROBLEM_H

#ifndef DD_OPTIMIZATION_PROGRAM_SOCKNAPSACKGUROBICLASS_H
#define DD_OPTIMIZATION_PROGRAM_SOCKNAPSACKGUROBICLASS_H

#include "gurobi_c++.h"
#include <vector>
#include <iostream>
#include "../AbstractGurobiProblems/AbstractProblemGurobiClass.h"
#include "SOCKnapsackInstance.h"

using namespace std;

/**
 * @brief MIP formulation for the SOC knapsack problem maximising Σ profit[i] * x[i]
 *        subject to second-order cone constraints.
 *
 * Variables:
 *   x[i]    ∈ {0,1} : item i is selected
 *   y[j][i] ≥ 0     : auxiliary variable — linearises sqrt_weights[j][i] * x[i]
 *   z[j]    ≥ 0     : auxiliary variable — represents the SOC right-hand side for constraint j
 *
 * Constraints:
 *   (1) y[j][i] = sqrt_weights[j][i] * x[i]                     — defines each y auxiliary variable
 *   (2) omega * z[j] = rhs[j] − Σᵢ linear_weights[j][i]*x[i]   — defines each z auxiliary variable
 *   (3) Σᵢ y[j][i]² ≤ z[j]²                                     — second-order cone constraint per j
 */
class SOCKnapsackGurobiClass : public AbstractProblemGurobi {
public:
    /**
     * @brief Constructor for SOCKnapsackGurobiClass.
     * @param params  SOCKnapsack instance (weights, rhs, variables, objective).
     * @param continuous  If true, x variables are continuous in [0,1]; otherwise binary.
     */
    SOCKnapsackGurobiClass(const SOCKnapsackInstance& params, bool continuous = false);

    /**
     * @brief Show the results of the optimization.
     */
    void show_results() override;

    /**
     * @brief Override: the SOCK model has auxiliary variables (y, z) not present in the DD.
     * Maps x_sol values to the DD's variables using the DD problem's variable name index.
     *
     * @param x_sol              Current LP relaxation values for self.x (model's x variables).
     * @param dd_ordered_variables Variable names in the DD's own ordering.
     * @param dd_name_to_index   Map from variable name to its index in the model's x.
     * @return Pair (x_sol_dd, var_ids): values and model indices aligned with the DD's variables.
     */
    pair<vector<double>, vector<int>> get_variable_values(
        const vector<double>& x_sol,
        const vector<int>& dd_to_original_index) const override;

private:
    const SOCKnapsackInstance params;   /**< Copy of the problem instance. */
    int num_constraints;                /**< Number of SOC constraints. */
    vector<vector<GRBVar>> y_vars;      /**< Auxiliary variables y[j][i]. */
    vector<GRBVar> z_vars;              /**< Auxiliary variables z[j]. */

    /**
     * @brief Add the SOC constraints to the Gurobi model.
     */
    void add_constraints() override;
};

#endif //DD_OPTIMIZATION_PROGRAM_SOCKNAPSACKGUROBICLASS_H

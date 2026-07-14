#ifndef DD_OPTIMIZATION_PROGRAM_KNAPSACKGUROBICLASS_H
#define DD_OPTIMIZATION_PROGRAM_KNAPSACKGUROBICLASS_H

#include "gurobi_c++.h"
#include <vector>
#include <iostream>
#include "../AbstractGurobiProblems/AbstractProblemGurobiClass.h"
#include "KnapsackInstance.h"

using namespace std;

/**
 * @brief MIP formulation for the 0-1 knapsack problem maximising Σ profit[i] * x[i].
 *
 * Variables:
 *   x[i] ∈ {0,1} : item i is selected
 *
 * Constraints:
 *   (1) Total weight of selected items must not exceed the knapsack capacity.
 */
class KnapsackGurobiClass : public AbstractProblemGurobi{
public:
    /**
     * @brief Constructor of the class KnapsackGurobiClass
     * @param params KnapsackInstance containing variables, weights, objective_weights and capacity.
     * @param continuous Boolean that indicates if the variables are continuous.
     */
    KnapsackGurobiClass(const KnapsackInstance& params, bool continuous = false);

    /**
     * @brief Show the results of the model.
     */
    void show_results() override;

private:
    vector<int> weights; /** Weights of the items. */
    int capacity; /** Capacity of the knapsack. */

    /**
     * @brief Add constraints to the model.
     */
    void add_constraints() override;
};

#endif //DD_OPTIMIZATION_PROGRAM_KNAPSACKGUROBICLASS_H

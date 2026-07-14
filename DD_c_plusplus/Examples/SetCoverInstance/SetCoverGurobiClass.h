#ifndef DD_OPTIMIZATION_PROGRAM_SetCoverGUROBICLASS_H
#define DD_OPTIMIZATION_PROGRAM_SetCoverGUROBICLASS_H

#include "gurobi_c++.h"
#include <vector>
#include <iostream>
#include "../AbstractGurobiProblems/AbstractProblemGurobiClass.h"
#include "SetCoverInstance.h"

using namespace std;

/**
 * @brief MIP formulation for the set cover problem minimising Σ cost[s] * x[s].
 *
 * Variables:
 *   x[s] ∈ {0,1} : subset s is selected
 *
 * Constraints:
 *   (1) Coverage: every element r must be covered by at least one selected subset.
 */
class SetCoverGurobiClass : public AbstractProblemGurobi {
public:
    /**
     * @brief Constructor of the class SetCoverGurobiClass
     * @param params SetCoverInstance containing variables, objective_weights and matrix_of_weight.
     * @param continuous Boolean that indicates if the variables are continuous.
     */
    SetCoverGurobiClass(const SetCoverInstance& params, bool continuous = false);

    /**
     * @brief Show the results of the model.
     */
    void show_results() override;


private:
    int restrictions_length; /** Number of restrictions in the model. */
    vector<vector<int>> restrictions; /** Matrix of restrictions. */

    /**
     * @brief Add constraints to the model.
     */
    void add_constraints() override;
};


#endif //DD_OPTIMIZATION_PROGRAM_SetCoverGUROBICLASS_H

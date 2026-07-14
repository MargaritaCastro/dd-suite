#ifndef DD_OPTIMIZATION_PROGRAM_INDEPENDENTSETGUROBICLASS_H
#define DD_OPTIMIZATION_PROGRAM_INDEPENDENTSETGUROBICLASS_H

#include "gurobi_c++.h"
#include <vector>
#include <iostream>
#include "../AbstractGurobiProblems/AbstractProblemGurobiClass.h"
#include "IndependentSetInstance.h"

using namespace std;
/**
 * @brief MIP formulation for the maximum weight independent set problem maximising Σ w[v] * x[v].
 *
 * Variables:
 *   x[v] ∈ {0,1} : vertex v is included in the independent set
 *
 * Constraints:
 *   (1) Edge exclusion: at most one endpoint of each edge can be selected.
 */
class IndependentSetGurobiClass : public AbstractProblemGurobi{

public:
    /**
     * @brief Constructor of the class IndependentSetGurobiClass
     * @param params IndependentSetInstance containing variables, objective_weights and edges.
     * @param continuous Boolean that indicates if the variables are continuous.
     */
    IndependentSetGurobiClass(const IndependentSetInstance& params, bool continuous = false);

    /**
     * @brief Show the results of the model.
     */
    void show_results() override;

private:
    vector<pair<int, int>> edges; /** Edges of the graph. */

    /**
     * @brief Add the constraints of the model.
     */
    void add_constraints() override;

};


#endif //DD_OPTIMIZATION_PROGRAM_INDEPENDENTSETGUROBICLASS_H

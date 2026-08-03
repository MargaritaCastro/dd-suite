#ifndef DD_OPTIMIZATION_PROGRAM_SEQUENCINGGUROBICLASS_H
#define DD_OPTIMIZATION_PROGRAM_SEQUENCINGGUROBICLASS_H

#include "gurobi_c++.h"
#include "../AbstractGurobiProblems/AbstractProblemGurobiClass.h"
#include "SequencingInstance.h"

#include <vector>
#include <map>
#include <utility>
#include <iostream>

using namespace std;

/**
 * @brief MIP formulation for 1|s_ij|Σ w_j*C_j (single-machine,
 *        sequence-dependent setup times, minimise weighted completion time).
 *
 * Variables:
 *   y[{DEPOT,j}] ∈ {0,1} : job j is first in the sequence  (DEPOT = n)
 *   y[{i,j}]     ∈ {0,1} : job i is immediately followed by job j (i≠j)
 *   C[j]         ≥ 0     : completion time of job j
 *
 * Constraints:
 *   (1) Depot has exactly one outgoing arc.
 *   (2) Each job has exactly one incoming arc.
 *   (3) Each job has at most one outgoing arc.
 *   (4) Big-M completion-time constraints (also serve as subtour elimination).
 */
class SequencingGurobiClass : public AbstractProblemGurobi {
public:
    explicit SequencingGurobiClass(const SequencingInstance& params, bool continuous = false);

    void optimize_with_cuts(vector<DD<SequencingState>*> dds,
                            CutType cut_type   = CutType::Target,
                            bool cut_strengthening = false);

    void show_results() override;

protected:
    void initialize_variables() override;
    void set_objective() override;
    void add_constraints() override;

private:
    int                         n_;
    int                         DEPOT_;     // = n
    vector<int>                 p_;         // processing times
    vector<int>                 w_;         // weights
    vector<vector<int>>         s_;         // setup_times (n+1) x n

    map<pair<int,int>, GRBVar>  y_;         // arc variables
    vector<GRBVar>              C_;         // completion times

    double big_M() const;
};

#endif //DD_OPTIMIZATION_PROGRAM_SEQUENCINGGUROBICLASS_H

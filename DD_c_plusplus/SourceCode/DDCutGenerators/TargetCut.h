#ifndef DD_OPTIMIZATION_PROGRAM_TARGETCUT_H
#define DD_OPTIMIZATION_PROGRAM_TARGETCUT_H

#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>
#include "gurobi_c++.h"
#include "DD.h"
#include "AbstractCutGenerator.h"
#include "MyExceptions.h"

using namespace std;

/**
 * @brief Target Cut generator (MDD version) from Algorithm 1 of:
 * Tjandraatmadja & van Hoeve, "Target Cuts from Relaxed Decision Diagrams",
 * INFORMS Journal on Computing, 2019.
 *
 * Supports general MDDs (Section 7). Given a relaxed MDD M representing a
 * set S and a fractional point x̄, solves the LP over P*_{ω,MDD}(M):
 *
 *     max  u^T (x̄ - ω)
 *     s.t. v_j ≤ v_i - ℓ·u_k   ∀ ℓ-arc (i,j) at layer k
 *          v_s = 1 + u^T ω,  v_t = 0
 *
 * where ω is the geometric center of S (Appendix B).
 *
 * If optimal LP value > 1: cut  u*^T x ≤ 1 + u*^T ω  (inequality).
 * If LP is unbounded along ray u*: equality  u*^T x = u*^T ω.
 * If optimal value ≤ 1: x̄ ∈ conv(S), no cut generated.
 *
 * @tparam T Type of the Node state values.
 */
template <typename T>
class TargetCut : public AbstractCutGenerator<T> {
public:
    explicit TargetCut(DD<T>* dd, double tolerance = 0.0001);
    ~TargetCut();

    double get_time() override;
    pair<vector<double>, double> get_cut() override;
    bool generate_cut(vector<double>& x_values, bool verbose = false) override;
    string get_name() override;

    bool is_equality_cut() const;

    vector<double> omega; /**< Geometric center of conv(S), one value per layer. */

private:
    Graph<T>* graph_DD;
    double time_;
    bool verbose_;
    bool is_equality_;
    int num_vars_;
    vector<unordered_map<int, int>> nodes_to_vars_;

    GRBEnv*   env_;
    GRBModel* model_;
    vector<GRBVar>              u_;  /**< Dual variables (one per layer, unbounded). */
    vector<vector<GRBVar>>      v_;  /**< Node potentials (unbounded). */

    vector<double> _compute_interior_point();
    void _build_model();
    bool _compute_cut(const vector<double>& x_values);
};

#include "TargetCut.tpp"
#endif //DD_OPTIMIZATION_PROGRAM_TARGETCUT_H

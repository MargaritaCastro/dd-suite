#ifndef DD_OPTIMIZATION_PROGRAM_ABSTRACTPROBLEMGUROBICLASS_H
#define DD_OPTIMIZATION_PROGRAM_ABSTRACTPROBLEMGUROBICLASS_H

#include "gurobi_c++.h"
#include "../../SourceCode/DD.h"
#include "../../SourceCode/DDCutGenerators/FlowCuts.h"
#include "../../SourceCode/DDCutGenerators/JointFlowCuts.h"
#include "../../SourceCode/DDCutGenerators/CutStrengthening.h"
#include "../../SourceCode/DDCutGenerators/TargetCut.h"
#include "../GuiStructure.h"
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <stdexcept>
#include <chrono>
#include <functional>
#include <memory>

using namespace std;

// Forward declaration so DDCutCallback can reference AbstractProblemGurobi
class AbstractProblemGurobi;

/**
 * @brief Gurobi callback that injects DD-based cuts at the root MIP node.
 *
 * Stores the parent problem by reference and accesses its type-erased
 * cut-generator functions (_cut_generate_fns, _cut_get_fns, etc.).
 */
class DDCutCallback : public GRBCallback {
public:
    explicit DDCutCallback(AbstractProblemGurobi& parent);
    void callback() override;
private:
    AbstractProblemGurobi& parent_;
};

/**
 * @brief Lightweight callback used by plain solves (no DD cuts): records the
 *        dual bound at the root node so the root gap can be computed afterwards.
 */
class RootBoundCallback : public GRBCallback {
public:
    explicit RootBoundCallback(AbstractProblemGurobi& parent);
    void callback() override;
private:
    AbstractProblemGurobi& parent_;
};

// ============================================================

class AbstractProblemGurobi {
public:

    /**
     * @brief Constructor for the AbstractProblem class.
     */
    AbstractProblemGurobi(int variable_lenght, vector<double> objective_weights,
                          bool continuous = false, int min_max = GRB_MAXIMIZE);

    /**
     * @brief Destructor for the AbstractProblem class.
     */
    virtual ~AbstractProblemGurobi();

    GRBModel*        model = nullptr;  /**< Gurobi model. */
    vector<GRBVar>   x;               /**< Decision variables of the model. */
    int              variable_length; /**< Number of variables in the model. */

    /** One-hour solver time limit, matching Castro, Cire & Beck (arXiv:2003.06363).
     *  Needed for the final gap to be meaningful: without it every instance solves
     *  to optimality and MIPGap is always 0. */
    static constexpr double TIME_LIMIT_SECONDS = 3600.0;

    /** Single-threaded so the branch-and-cut search is reproducible: Gurobi's
     *  parallel determinism depends on a fixed thread count, and a wall-clock
     *  TimeLimit under shared-cluster load makes multi-threaded runs flip
     *  borderline instances between solved/unsolved. */
    static constexpr int THREADS = 1;

    double root_bound     = 0.0;   /**< Dual bound at the root node (after cuts). */
    bool   root_bound_set = false; /**< Whether root_bound was captured this solve. */

    /**
     * @brief Optimize the model without DD cuts (baseline).
     */
    virtual void optimize_model();

    /**
     * @brief Initialize cut generators and strengtheners for each DD.
     *
     * Mirrors Python's _set_up_dd_cut_generators. Called by optimize_with_cuts.
     * Can also be called standalone before optimize_model() if needed.
     *
     * @tparam T  State type of the DD.
     * @param dds  Vector of DDs used to generate cuts.
     */
    template<typename T>
    void set_up_dd_cut_generators(vector<DD<T>*> dds);

    /**
     * @brief Optimize using DD-based cuts injected via Gurobi callback.
     *
     * Sets joint_flow_cuts and cut_strengthening, calls set_up_dd_cut_generators,
     * then runs Gurobi with a MIP node callback.
     *
     * @tparam T  State type of the DD (e.g. int, dynamic_bitset<>, vector<double>).
     * @param dds        Vector of DDs used to generate cuts.
     * @param joint_flow If true, use JointFlowCuts; otherwise FlowCuts.
     * @param cut_strengthening  If true, apply cut strengthening after generation.
     */
    template<typename T>
    void optimize_with_cuts(vector<DD<T>*> dds,
                            CutType cut_type    = CutType::JointFlow,
                            bool cut_strengthening  = false);

    /**
     * @brief Hook called inside optimize_with_cuts before model->optimize().
     * Override in subclasses to set problem-specific solver parameters.
     */
    virtual void configure_cuts_params() {}

    /**
     * @brief Show the results of the model.
     */
    virtual void show_results();

    /**
     * @brief Map the model's x solution to the variable values expected by a given DD.
     *
     * Default implementation is identity: returns (x_sol, {0, 1, ..., n-1}).
     * Override in subclasses where the model has auxiliary variables not present
     * in the DD (e.g. SOCKnapsack has y and z), or where the variable ordering differs.
     *
     * @param x_sol                Current LP relaxation values for self.x (model variables).
     * @param dd_to_original_index Maps DD layer position i → original model variable index.
     * @return Pair (x_sol_dd, var_ids): values and model indices aligned with the DD.
     */
    virtual pair<vector<double>, vector<int>> get_variable_values(
        const vector<double>& x_sol,
        const vector<int>& dd_to_original_index) const;

    /**
     * @brief Add additional cuts to the model.
     * @param cuts Cuts to add.
     */
    void add_additional_cuts(vector<pair<vector<double>, double>> cuts);

    /**
     * @brief Create the model, initialize the variables, add the cuts, and set
     *        the objective function.
     */
    void create_model();

    /**
     * @brief Get the time to optimize the Gurobi model.
     * @return String representation of the time (seconds).
     */
    string get_optimization_time();

    /**
     * @brief Root node gap as defined by Castro, Cire & Beck (arXiv:2003.06363):
     *        relative gap between the dual bound at the root node (after cuts) and
     *        the optimal integer objective, as a fraction (multiply by 100 for %).
     *        Returns 0.0 when the root bound was not captured (e.g. LP models) or
     *        the optimum is ~0.
     */
    double get_root_gap();

    /**
     * @brief Final optimality gap at termination (Gurobi MIPGap), as a fraction.
     *        The paper's "Final Gap": 0 when solved to optimality, > 0 when the
     *        solver stops at the time limit. Returns 0.0 for continuous/LP models.
     */
    double get_final_gap();

protected:
    vector<pair<vector<double>, double>> cuts; /**< Cuts to add to the model. */
    bool           continuous;                 /**< Variables are continuous? */
    GRBEnv*        env = nullptr;              /**< Gurobi environment. */
    vector<double> objective_weights;          /**< Objective function weights. */
    int            min_max;                    /**< GRB_MINIMIZE or GRB_MAXIMIZE. */
    chrono::duration<double> optimization_time = chrono::duration<double>(0);

    void initialize_model();
    virtual void initialize_variables();
    void add_cuts();
    virtual void set_objective();
    virtual void add_constraints();

    // ---- DD cut settings ----
    CutType _cut_type       = CutType::JointFlow;
    bool _use_cut_strengthening = false;
public:
    int num_cuts_added = 0;
protected:

    // ---- Type-erased cut-generator state (populated by set_up_dd_cut_generators<T>) ----
    vector<function<bool(vector<double>&)>>                                    _cut_generate_fns;
    vector<function<pair<vector<double>, double>()>>                           _cut_get_fns;
    vector<function<bool(const vector<double>&, double)>>                      _cut_strengthening_fns;
    vector<function<pair<vector<double>, double>()>>                           _cut_strengthening_get_fns;
    vector<function<pair<vector<double>, vector<int>>(const vector<double>&)>> _variable_mapping_fns;
    vector<std::shared_ptr<void>> _cut_generator_owners; /**< Keeps generators alive during optimize(). */

    friend class DDCutCallback;
};


// ============================================================
// Template implementation — must be visible at instantiation point
// ============================================================

template<typename T>
void AbstractProblemGurobi::set_up_dd_cut_generators(vector<DD<T>*> dds) {
    _cut_generate_fns.clear();
    _cut_get_fns.clear();
    _cut_strengthening_fns.clear();
    _cut_strengthening_get_fns.clear();
    _variable_mapping_fns.clear();
    _cut_generator_owners.clear();

    for (auto* dd : dds) {
        vector<int> orig_idx = dd->problem.dd_to_original_index;
        _variable_mapping_fns.push_back(
            [this, orig_idx](const vector<double>& x_sol) {
                return this->get_variable_values(x_sol, orig_idx);
            }
        );

        // Cut generator — resolved from _cut_type
        if (_cut_type == CutType::Target) {
            auto gen = std::make_shared<TargetCut<T>>(dd);
            _cut_generator_owners.push_back(gen);
            _cut_generate_fns.push_back([gen](vector<double>& x) { return gen->generate_cut(x); });
            _cut_get_fns.push_back([gen]() { return gen->get_cut(); });
        } else if (_cut_type == CutType::JointFlow) {
            auto gen = std::make_shared<JointFlowCuts<T>>(dd);
            _cut_generator_owners.push_back(gen);
            _cut_generate_fns.push_back([gen](vector<double>& x) { return gen->generate_cut(x); });
            _cut_get_fns.push_back([gen]() { return gen->get_cut(); });
        } else {
            auto gen = std::make_shared<FlowCuts<T>>(dd);
            _cut_generator_owners.push_back(gen);
            _cut_generate_fns.push_back([gen](vector<double>& x) { return gen->generate_cut(x); });
            _cut_get_fns.push_back([gen]() { return gen->get_cut(); });
        }

        // Cut strengthener
        if (_use_cut_strengthening) {
            auto strengthener = std::make_shared<CutStrengthening<T>>(dd);
            _cut_generator_owners.push_back(strengthener);
            _cut_strengthening_fns.push_back([strengthener](const vector<double>& c, double k) {
                return strengthener->cut_strengthening(c, k);
            });
            _cut_strengthening_get_fns.push_back([strengthener]() { return strengthener->get_cut_strengthening(); });
        }
    }
}

template<typename T>
void AbstractProblemGurobi::optimize_with_cuts(vector<DD<T>*> dds,
                                               CutType cut_type,
                                               bool cut_strengthening_flag) {

    auto start = chrono::steady_clock::now();

    _cut_type          = cut_type;
    _use_cut_strengthening = cut_strengthening_flag;
    num_cuts_added = 0;
    root_bound_set = false;

    set_up_dd_cut_generators<T>(dds);

    model->set(GRB_IntParam_PreCrush, 1);
    configure_cuts_params();
    model->update();

    DDCutCallback cb(*this);
    model->setCallback(&cb);

    model->optimize();
    model->setCallback(nullptr);
    show_results();
    auto end = chrono::steady_clock::now();
    optimization_time = chrono::duration<double>(end - start);
}

#endif //DD_OPTIMIZATION_PROGRAM_ABSTRACTPROBLEMGUROBICLASS_H

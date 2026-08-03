#ifndef DD_OPTIMIZATION_PROGRAM_JointFlowCuts_H
#define DD_OPTIMIZATION_PROGRAM_JointFlowCuts_H

#include <vector>
#include <unordered_map>
#include <iostream>
#include <ctime>
#include <stdexcept>
#include "DD.h"
#include "AbstractCutGenerator.h"

using namespace std;

/**
 * @brief JointFlowCuts class, responsible for solving the cut problem in a graph, utilizing the dual of the linear programming
    model to find the cut.
    This algorithm can only be used to solve binary decision diagrams (BDDs) and was specifically designed
    to address certain max-flow problems from the paper "A combinatorial cut-and-lift procedure with an
    application to 0–1 second-order conic programming. Mathematical Programming, section 5.2."

    Castro, M. P., Cire, A. A., & Beck, J. C. (2022). A combinatorial cut-and-lift procedure with an
    application to 0–1 second-order conic programming. Mathematical Programming, 196(1), 115-171.
 * @tparam T Type of the Node state values
 */

template <typename T>
class JointFlowCuts : public AbstractCutGenerator<T> {
public:
    /**
     * @brief Constructor for the Dual Cuts class.
     * @param DD Decision Diagram to be solved.
     */
    JointFlowCuts(DD<T>* DD);

    /**
     * @brief Destructor of the JointFlowCuts class
     */
    ~JointFlowCuts();

    /**
     * @brief Returns the execution time of the cut algorithm.
     * @return double: Time taken to solve the cut problem.
     */
    double get_time() override;

    /**
     * @brief Returns the precomputed values from the generate_cut() method,
     * this cut can be used to take out some options of the optimization problem.
     * @return Tuple with the found cut and the value of the cut.
     * The cut is represented by a vector of coefficients and a constant.
     * As the form of the cut is: coefficients * x <= constant.
     */
    pair<vector<double>, double> get_cut() override;
    /**
     * @brief Solves the decision diagram, finding a cut.
     * @param x_values List of x values in the graph. (Equivalent to capacities)
     * @param verbose Boolean to print additional information.
     * @return bool: if a cut is found
     */
    bool generate_cut(vector<double>& x_values, bool verbose = false) override;

    /**
 * @brief Returns the name of the cut generator.
 * @return String representation of the cut generator.
 */
    string get_name() override;

private:
    Graph<T>* graph_DD; /** Graph representation of the decision diagram */
    bool verbose; /** Boolean to print additional information */
    double time; /** Time taken to solve the cut problem */
    double tolerance; /** Tolerance value for the cut */
    vector<double> x_values; /** Values of the variables */
    GRBEnv* env; /** Environment of the model. */
    GRBModel* model; /** Gurobi model */
    vector<GRBVar> mu; /** Dual 0-Variables */
    vector<GRBVar> lambda_vars; /** Dual 1-Variables */
    vector<vector<GRBVar>> w; /** Arc flow variables*/
    vector<map<int, int>> nodes_to_vars; /** Map between graph nodes and gurobi model */

    /**
     * @brief Solves the decision diagram, finding a cut. Returns this value.
     * @param x_values: List of x values in the graph. (Equivalent to capacities)
     * @return bool: if a cut is found
     */
    bool compute_cut(const vector<double>& x_values);
    /**
     * @brief Creates the linear programming model to be solved.
     */
    void update_lp_model();
    /**
     * @brief Sets the variables for the linear programming model.
     */
    void set_variables();
    /**
     * @brief Sets the constraints for the linear programming model.
     */
    void set_constraints();
    /**
     * @brief Solves the linear programming model.
     * @return bool: if the model was solved.
     */
    bool solve_lp_model();
    /**
     * @brief Sets the objective function for the linear programming model.
     */
    void set_objective();
};

#include "JointFlowCuts.tpp"
#endif //DD_OPTIMIZATION_PROGRAM_JointFlowCuts_H

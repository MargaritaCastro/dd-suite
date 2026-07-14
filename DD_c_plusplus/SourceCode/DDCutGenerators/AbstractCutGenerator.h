#ifndef DD_OPTIMIZATION_PROGRAM_ABSTRACTCUTGENERATOR_H
#define DD_OPTIMIZATION_PROGRAM_ABSTRACTCUTGENERATOR_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <tuple>

using namespace std;

/**
 * @brief Abstract class that provides the foundation for generating cuts.
 * @tparam T Type of the Node state values
 */
template <typename T>
class AbstractCutGenerator {
public:
    /**
     * @brief Constructor for the AbstractCutGenerator class.
     */
    AbstractCutGenerator();

    /**
     * @brief Returns the execution time of the cut algorithm.
     * @return Execution time of the cut algorithm.
     */
    virtual double get_time() = 0;

    /**
     * @brief Returns the cut found by the algorithm, this cut can be used to
     * take out some options of the optimization problem.
     * @return Tuple with the found cut and the value of the cut.
     * The cut is represented by a vector of coefficients and a constant.
     * As the form of the cut is: coefficients * x <= constant.
     */
    virtual pair<vector<double>, double> get_cut() = 0;

    /**
     * @brief Generates a cut for the optimization problem.
     * @param x_values Decision variable values.
     * @param verbose Boolean indicating whether to print additional information.
     * @return Boolean indicating whether a cut was generated.
     */
    virtual bool generate_cut(vector<double>& x_values, bool verbose=false) = 0;

    /**
     * @brief Returns the name of the cut generator.
     * @return String representation of the cut generator.
     */
    virtual string get_name() = 0;

    /**
     * @brief Destructor for the AbstractCutGenerator class.
     */
    virtual ~AbstractCutGenerator()= default;

protected:
    vector<double> coefficients; /** Coefficients of the cut */
    double constant; /** Constant of the cut */
    double tolerance = 1e-5; /** Tolerance value for the cut, to avoid numeric errors */

    /**
     * @brief Returns the inequality of the cut.
     * @return String nequality of the cut.
     */
    string get_inequality_string_representation() const;
};

#include "AbstractCutGenerator.tpp"
#endif //DD_OPTIMIZATION_PROGRAM_ABSTRACTCUTGENERATOR_H

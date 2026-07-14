#ifndef DD_OPTIMIZATION_PROGRAM_SOCKNAPSACKINSTANCE_H
#define DD_OPTIMIZATION_PROGRAM_SOCKNAPSACKINSTANCE_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

/**
 * @brief Class for reading and storing a SOC (Second-Order Cone) Knapsack instance.
 *
 * The state is represented as a vector<double> of size 2:
 *   [0] = accumulated linear weight sum
 *   [1] = accumulated weight sum inside the square-root term
 *
 */
class SOCKnapsackInstance {
public:
    /**
     * @brief Constructor for the SOCKnapsackInstance class.
     * @param file_path Path to the instance file.
     */
    explicit SOCKnapsackInstance(const string& file_path);
    SOCKnapsackInstance(const SOCKnapsackInstance& other);
    SOCKnapsackInstance& operator=(const SOCKnapsackInstance& other);
    ~SOCKnapsackInstance() { delete initial_state; }

    vector<double>* initial_state;          /**< Initial state of the problem: {0.0, 0.0}. */
    int omega;                              /**< Scaling factor for the square-root term. */
    vector<int> right_side_of_restrictions; /**< Right-hand side of each SOC constraint. */
    vector<double> objective_weights;       /**< Objective coefficient for each variable. */
    vector<vector<int>> linear_weights;     /**< Linear weight per constraint per variable: linear_weights[constraint][variable]. */
    vector<vector<int>> sqrt_weights;       /**< Weight inside the square-root term per constraint per variable: sqrt_weights[constraint][variable]. */
    vector<pair<string, vector<int>>> variables; /**< Variable names and their binary domains. */

    /**
     * @brief Reads and parses the instance file.
     * @param file_path Path to the instance file.
     */
    void read_file(const string& file_path);
};

#endif //DD_OPTIMIZATION_PROGRAM_SOCKNAPSACKINSTANCE_H

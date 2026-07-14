#ifndef DD_OPTIMIZATION_PROGRAM_SCHEDULERINSTANCE_H
#define DD_OPTIMIZATION_PROGRAM_SCHEDULERINSTANCE_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "SchedulerState.h"

using namespace std;

/**
 * @brief Reads a single-machine scheduling instance from file.
 *
 * File format:
 *   n_jobs
 *   p_0 p_1 ... p_{n-1}          (processing times)
 *   w_0 w_1 ... w_{n-1}          (weights)
 *   s_00 s_01 ... s_0{n-1}       (setup times from depot, row 0)
 *   s_10 ...                     (setup times from job 0, row 1)
 *   ...
 *   s_n0 ...                     (setup times from job n-1, row n)
 *
 * Problem: minimise Σ w_j * C_j  (1 | s_ij | Σ w_j C_j)
 */
class SchedulerInstance {
public:
    explicit SchedulerInstance(const string& file_path);

    int                            n_jobs;
    vector<int>                    processing_times;
    vector<int>                    weights;
    vector<vector<int>>            setup_times;      // (n+1) x n
    vector<double>                 objective_weights; // = weights (cast to double)
    vector<pair<string, vector<int>>> variables;      // slot_0..slot_{n-1}, domain {0..n-1}
    SchedulerState*                initial_state;

    ~SchedulerInstance() { delete initial_state; }

private:
    void read_file(const string& file_path);
};

#endif //DD_OPTIMIZATION_PROGRAM_SCHEDULERINSTANCE_H

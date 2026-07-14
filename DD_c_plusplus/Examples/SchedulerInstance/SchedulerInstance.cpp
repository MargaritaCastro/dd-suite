#include "SchedulerInstance.h"

SchedulerInstance::SchedulerInstance(const string& file_path) {
    read_file(file_path);
}

void SchedulerInstance::read_file(const string& file_path) {
    ifstream input(file_path);
    if (!input.is_open()) {
        cerr << "Error: could not open file " << file_path << endl;
        exit(1);
    }

    input >> n_jobs;

    processing_times.resize(n_jobs);
    for (int j = 0; j < n_jobs; ++j)
        input >> processing_times[j];

    weights.resize(n_jobs);
    for (int j = 0; j < n_jobs; ++j)
        input >> weights[j];

    // (n+1) rows x n columns: row 0 = from depot, rows 1..n = from job 0..n-1
    setup_times.resize(n_jobs + 1, vector<int>(n_jobs));
    for (int i = 0; i <= n_jobs; ++i)
        for (int j = 0; j < n_jobs; ++j)
            input >> setup_times[i][j];

    // objective weights (double cast of weights)
    objective_weights.resize(n_jobs);
    for (int j = 0; j < n_jobs; ++j)
        objective_weights[j] = static_cast<double>(weights[j]);

    // slot_k ∈ {0, ..., n-1}
    vector<int> domain(n_jobs);
    for (int j = 0; j < n_jobs; ++j) domain[j] = j;
    for (int k = 0; k < n_jobs; ++k)
        variables.emplace_back("slot_" + to_string(k), domain);

    // initial state: empty set, last=-1 (depot)
    initial_state = new SchedulerState{{}, -1};
}

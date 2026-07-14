
#ifndef DD_OPTIMIZATION_PROGRAM_KNAPSACKINSTANCE_H
#define DD_OPTIMIZATION_PROGRAM_KNAPSACKINSTANCE_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <tuple>
#include <fstream>

using namespace std;

class KnapsackInstance {
public:
    KnapsackInstance(const string& file_path);
    KnapsackInstance(int* initial_state,
                     vector<pair<string, vector<int>>> variables,
                     vector<int> weights,
                     vector<double> objective_weights,
                     int right_side_of_restrictions);
    ~KnapsackInstance() { delete initial_state; }

    int* initial_state;
    int right_side_of_restrictions;
    vector<int> weights;
    vector<double> objective_weights;
    vector<pair<string, vector<int>>> variables;

    void read_file(const string& file_path);

};

#endif //DD_OPTIMIZATION_PROGRAM_KNAPSACKINSTANCE_H

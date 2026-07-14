#ifndef DD_OPTIMIZATION_PROGRAM_SetCoverINSTANCE_H
#define DD_OPTIMIZATION_PROGRAM_SetCoverINSTANCE_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <tuple>
#include <boost/dynamic_bitset.hpp>
#include <filesystem>
#include <fstream>

using namespace std;
using namespace boost;

class SetCoverInstance {
public:
    SetCoverInstance(const string& file_path);
    SetCoverInstance(dynamic_bitset<>* initial_state,
                     vector<pair<string, vector<int>>> variables,
                     vector<vector<int>> matrix_of_weight,
                     vector<int> right_side_of_restrictions);
    ~SetCoverInstance() { delete initial_state; }

    dynamic_bitset<>* initial_state;
    vector<std::pair<string, vector<int>>> variables;
    vector<double> objective_weights;
    vector<vector<int>> matrix_of_weight;
    vector<int> right_side_of_restrictions;

    void read_file(const string& file_path);
};

#endif //DD_OPTIMIZATION_PROGRAM_SetCoverINSTANCE_H

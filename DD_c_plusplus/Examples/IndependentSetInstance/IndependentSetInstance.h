#ifndef DD_OPTIMIZATION_PROGRAM_INDEPENDENTSETINSTANCE_H
#define DD_OPTIMIZATION_PROGRAM_INDEPENDENTSETINSTANCE_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <tuple>
#include <map>
#include <boost/dynamic_bitset.hpp>
#include <fstream>

using namespace std;
using namespace boost;

class IndependentSetInstance {
public:
    IndependentSetInstance(const string& file_path);
    IndependentSetInstance(dynamic_bitset<>* initial_state,
                           vector<pair<string, vector<int>>> variables,
                           map<string, vector<int>> dict_node_neighbors);
    ~IndependentSetInstance() { delete initial_state; }

    vector<pair<string, vector<int>>> variables;
    vector<double> objective_weights;
    map<string, vector<int>> dict_node_neighbors;
    dynamic_bitset<>* initial_state;
    vector<pair<int, int>> edges;

    void read_file(const string& file_path);
};

#endif //DD_OPTIMIZATION_PROGRAM_INDEPENDENTSETINSTANCE_H

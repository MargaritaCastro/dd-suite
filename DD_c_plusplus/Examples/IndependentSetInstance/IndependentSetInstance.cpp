#include "IndependentSetInstance.h"

IndependentSetInstance::IndependentSetInstance(const string& file_path) {
    read_file(file_path);
}

IndependentSetInstance::IndependentSetInstance(dynamic_bitset<>* initial_state,
                                               vector<pair<string, vector<int>>> variables,
                                               map<string, vector<int>> dict_node_neighbors)
    : initial_state(initial_state),
      variables(std::move(variables)),
      dict_node_neighbors(std::move(dict_node_neighbors))
{}

void IndependentSetInstance::read_file(const string &file_path) {
    ifstream input(file_path);
    if( !input.is_open() ) {
        cerr << "Error: could not open file " << file_path << endl;
        exit(1);
    }

    int variable_length;

    input >> variable_length;
    for (int i = 1; i <= variable_length; ++i) {
        string variable_name = "x_" + to_string(i);
        variables.emplace_back(make_pair(variable_name, vector<int>{0,1}));
    }

    initial_state = new dynamic_bitset<>(variable_length);
    for (int i = 0; i < variable_length; ++i) {
        initial_state->set(i);
    }


    for (int i = 0; i < variable_length; i++) {
        double value;
        input >> value;
        objective_weights.push_back(value);
    }

    for (int i = 0; i < variable_length; ++i) {
        dict_node_neighbors["x_" + to_string(i+1)] = vector<int>();
    }

    string line;
    getline(input, line);

    for (int i = 0; i <= variable_length; i++) {
        getline(input, line);
        stringstream ss(line);
        string key;
        char colon;
        ss >> key >> colon;

        key.pop_back();

        vector<int> values;
        int value;
        char comma;

        while (ss >> value >> comma) {
            values.push_back(value);
        }
        ss >> value;

        dict_node_neighbors[key] = values;

        int child_node = stoi(key.substr(2));

        for (int parent_node : values) {
            if (child_node < parent_node) {
                edges.emplace_back(child_node, parent_node);
            }
        }
    }

}
#include "KnapsackInstance.h"


KnapsackInstance::KnapsackInstance(const string& file_path) {
    read_file(file_path);
}

KnapsackInstance::KnapsackInstance(int* initial_state,
                                   vector<pair<string, vector<int>>> variables,
                                   vector<int> weights,
                                   vector<double> objective_weights,
                                   int right_side_of_restrictions)
    : initial_state(initial_state),
      right_side_of_restrictions(right_side_of_restrictions),
      weights(std::move(weights)),
      objective_weights(std::move(objective_weights)),
      variables(std::move(variables))
{}

void KnapsackInstance::read_file(const string& file_path) {
    ifstream input(file_path);
    if( !input.is_open() ) {
        cerr << "Error: could not open file " << file_path << endl;
        exit(1);
    }

    initial_state = new int(0);
    int variable_length;

    input >> variable_length;

    input >> right_side_of_restrictions;

    for (int j = 0; j < variable_length; j++) {
        int value;
        input >> value;
        weights.push_back(value);
    }


    for (int i = 0; i < variable_length; i++) {
        double value;
        input >> value;
        objective_weights.push_back(value);
    }

    for (int i = 1; i <= variable_length; ++i) {
        string variable_name = "x_" + to_string(i);
        variables.emplace_back(make_pair(variable_name, vector<int>{0,1}));
    }
}
#include "SetCoverInstance.h"

SetCoverInstance::SetCoverInstance(const string& file_path) {
    read_file(file_path);
}

SetCoverInstance::SetCoverInstance(dynamic_bitset<>* initial_state,
                                   vector<pair<string, vector<int>>> variables,
                                   vector<vector<int>> matrix_of_weight,
                                   vector<int> right_side_of_restrictions)
    : initial_state(initial_state),
      variables(std::move(variables)),
      matrix_of_weight(std::move(matrix_of_weight)),
      right_side_of_restrictions(std::move(right_side_of_restrictions))
{}

void SetCoverInstance::read_file(const string &file_path) {
    ifstream input(file_path);
    if( !input.is_open() ) {
        cerr << "Error: could not open file " << file_path << endl;
        exit(1);
    }

    int variable_length;
    int restrictions_length;

    input >> variable_length;
    input >> restrictions_length;

    initial_state = new dynamic_bitset<>(restrictions_length);
    for (int i = 0; i < restrictions_length; ++i) {
        initial_state->set(i);
    }

    right_side_of_restrictions = vector<int>(restrictions_length, 1);


    for (int i = 1; i <= variable_length; ++i) {
        std::string variable_name = "x_" + std::to_string(i);
        variables.emplace_back(variable_name, vector<int>{0,1});
    }

    for (int i = 0; i < variable_length; i++) {
        int value;
        input >> value;
        objective_weights.push_back(value);
    }

    for (int i = 0; i < restrictions_length; i++) {
        vector<int> row;
        for (int j = 0; j < variable_length; j++) {
            int value;
            input >> value;
            row.push_back(value);
        }
        matrix_of_weight.push_back(row);
    }

}

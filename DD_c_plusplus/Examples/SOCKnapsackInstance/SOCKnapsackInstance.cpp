#include "SOCKnapsackInstance.h"


SOCKnapsackInstance::SOCKnapsackInstance(const string& file_path) {
    read_file(file_path);
}

SOCKnapsackInstance::SOCKnapsackInstance(const SOCKnapsackInstance& other)
    : omega(other.omega)
    , right_side_of_restrictions(other.right_side_of_restrictions)
    , objective_weights(other.objective_weights)
    , linear_weights(other.linear_weights)
    , sqrt_weights(other.sqrt_weights)
    , variables(other.variables)
    , initial_state(other.initial_state ? new vector<double>(*other.initial_state) : nullptr)
{}

SOCKnapsackInstance& SOCKnapsackInstance::operator=(const SOCKnapsackInstance& other) {
    if (this == &other) return *this;
    delete initial_state;
    omega = other.omega;
    right_side_of_restrictions = other.right_side_of_restrictions;
    objective_weights = other.objective_weights;
    linear_weights = other.linear_weights;
    sqrt_weights = other.sqrt_weights;
    variables = other.variables;
    initial_state = other.initial_state ? new vector<double>(*other.initial_state) : nullptr;
    return *this;
}

void SOCKnapsackInstance::read_file(const string& file_path) {
    ifstream input(file_path);
    if (!input.is_open()) {
        cerr << "Error: could not open file " << file_path << endl;
        exit(1);
    }

    int variable_length, num_constraints;
    input >> variable_length >> num_constraints;

    input >> omega;

    // Read objective coefficients, one per variable
    for (int i = 0; i < variable_length; i++) {
        double value;
        input >> value;
        objective_weights.push_back(value);
    }

    // Read right-hand side, one per constraint
    for (int i = 0; i < num_constraints; i++) {
        int value;
        input >> value;
        right_side_of_restrictions.push_back(value);
    }

    // Read linear weight matrix: one row per constraint, one column per variable
    for (int i = 0; i < num_constraints; i++) {
        vector<int> row;
        for (int j = 0; j < variable_length; j++) {
            int value;
            input >> value;
            row.push_back(value);
        }
        linear_weights.push_back(row);
    }

    // Read square-root weight matrix: one row per constraint, one column per variable
    for (int i = 0; i < num_constraints; i++) {
        vector<int> row;
        for (int j = 0; j < variable_length; j++) {
            int value;
            input >> value;
            row.push_back(value);
        }
        sqrt_weights.push_back(row);
    }

    for (int i = 1; i <= variable_length; ++i) {
        string variable_name = "x_" + to_string(i);
        variables.emplace_back(make_pair(variable_name, vector<int>{0, 1}));
    }

    initial_state = new vector<double>{0.0, 0.0};
}



#include "SetCoverProblem.h"

#include <numeric>
#include <utility>

SetCoverProblem::SetCoverProblem(const SetCoverInstance& params, bool sort)
        : AbstractProblem(params.initial_state, params.variables)
{
    _raw_matrix = params.matrix_of_weight;

    // Identity default (overwritten by sort_variables when sort=true)
    dd_to_original_index.resize(params.variables.size());
    iota(dd_to_original_index.begin(), dd_to_original_index.end(), 0);

    if (sort) {
        apply_variable_order(sort_variables(params.variables));
    }

    dynamic_bitset<>* bs = convert_to_bitset(params.right_side_of_restrictions, _raw_matrix.size());
    this->right_side_of_restrictions = *bs;
    delete bs;

    for (const auto& row : _raw_matrix) {
        dynamic_bitset<>* bsr = convert_to_bitset(row, _raw_matrix[0].size());
        this->matrix_of_weight.push_back(*bsr);
        delete bsr;
    }

    create_transpose_matrix_of_weights(_raw_matrix);
    check_atributes(params.variables, params.initial_state);
    calculate_row_max_index();
}

vector<string> SetCoverProblem::sort_variables(const vector<pair<string, vector<int>>>& variables) {
    int n_vars = static_cast<int>(variables.size());
    int n_rows = static_cast<int>(_raw_matrix.size());

    vector<int> coverage(n_vars, 0);
    for (int j = 0; j < n_vars; ++j)
        for (int i = 0; i < n_rows; ++i)
            coverage[j] += _raw_matrix[i][j];

    vector<int> sort_indices(n_vars);
    iota(sort_indices.begin(), sort_indices.end(), 0);
    stable_sort(sort_indices.begin(), sort_indices.end(),
        [&coverage](int a, int b) { return coverage[a] > coverage[b]; });
    dd_to_original_index = sort_indices;

    vector<vector<int>> new_matrix(n_rows, vector<int>(n_vars));
    for (int i = 0; i < n_rows; ++i)
        for (int new_j = 0; new_j < n_vars; ++new_j)
            new_matrix[i][new_j] = _raw_matrix[i][sort_indices[new_j]];
    _raw_matrix = new_matrix;

    vector<string> sorted_names;
    sorted_names.reserve(n_vars);
    for (int idx : sort_indices)
        sorted_names.push_back(variables[idx].first);
    return sorted_names;
}

void SetCoverProblem::create_transpose_matrix_of_weights(vector<vector<int>>& vector_matrix_of_weight) {

    size_t rows = vector_matrix_of_weight.size();
    size_t var_num = vector_matrix_of_weight[0].size();
    matrix_of_weight_transpose.assign(var_num, dynamic_bitset<>(rows));

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < var_num; ++j) {
            if(vector_matrix_of_weight[i][j] == 1){
                matrix_of_weight_transpose[j].set(i);
            }
        }
    }

}

dynamic_bitset<>* SetCoverProblem::convert_to_bitset(const vector<int>& vec, int size) {
    dynamic_bitset<>* bs = new dynamic_bitset<>(size);

    for (size_t i = 0; i < vec.size(); ++i) {
        (*bs)[i] = vec[i];
    }

    return bs;
}

void SetCoverProblem::check_atributes(const vector<pair<string, vector<int>>>& variables, dynamic_bitset<>* initial_state) {
    check_same_len_matrix_and_right_side(initial_state);
    check_same_len_rows_matrix_and_variables(variables);
}

void SetCoverProblem::check_same_len_matrix_and_right_side(dynamic_bitset<>* initial_state) {
    assert(matrix_of_weight.size() == right_side_of_restrictions.size() && "weights and capacity must have the same length");

    assert((initial_state->count() == right_side_of_restrictions.size() || initial_state->count() == 2 * right_side_of_restrictions.size()) &&
           "initial_state and capacity must have the same length");
}

void SetCoverProblem::check_same_len_rows_matrix_and_variables(const vector<pair<string, vector<int>>>& variables) {
    for (auto &row: matrix_of_weight) {
        assert(variables.size() == row.size() &&
               "weights and variables must have the same length");
    }
}

void SetCoverProblem::calculate_row_max_index() {
    max_indices.clear();
    for (size_t row = 0; row < matrix_of_weight.size(); ++row) {
        int max_index = -1;
        for (int col = matrix_of_weight[row].size() - 1; col >= 0; --col) {
            if (matrix_of_weight[row][col] == 1) {
                max_index = col + 1;
                break;
            }
        }
        max_indices.push_back(max_index);
    }
}

bool SetCoverProblem::transition_function(const dynamic_bitset<>* previous_state, const int variable_index, int variable_value, dynamic_bitset<>* scratch_state) const {
    if (variable_value == 0) {
        for (size_t row = 0; row < previous_state->size(); ++row) {
            if (max_indices[row] != -1 && variable_index >= max_indices[row] - 1) {
                if (previous_state->test(row))
                    return false;
            }
        }
        *scratch_state = *previous_state;
        return true;
    }
    *scratch_state = *previous_state;
    *scratch_state -= matrix_of_weight_transpose[variable_index];
    return true;
}

int SetCoverProblem::get_priority_for_discard_node(const dynamic_bitset<>* state) const {
    return state->count();
}

int SetCoverProblem::get_priority_for_merge_nodes(const int node_id, const dynamic_bitset<>* state) const {
    return state->count();
}

dynamic_bitset<>* SetCoverProblem::merge_operator(const dynamic_bitset<>* state_one, const dynamic_bitset<>* state_two) const {
    dynamic_bitset<>* merged_state = new dynamic_bitset<>((*state_one) & (*state_two));
    return merged_state;
}

string SetCoverProblem::get_state_as_string(const dynamic_bitset<>* state) const {
    string state_string;
    boost::to_string(*state, state_string);
    return state_string;
}

dynamic_bitset<>* SetCoverProblem::get_state_copy(const dynamic_bitset<>* state) const {
    dynamic_bitset<>* copy_bitset = new dynamic_bitset<>(*state);
    return copy_bitset;
}

dynamic_bitset<>* SetCoverProblem::get_final_state() {
    vector<int> final_state(matrix_of_weight.size(), 0);
    return convert_to_bitset(final_state, matrix_of_weight.size());
}
#include "AbstractProblemClass.h"
#include <numeric>

template <typename T>
AbstractProblem<T>::AbstractProblem(T* initial_state, const vector<pair<string, vector<int>>> &variables):
        initial_state(initial_state)
{
    get_variables(variables);
}

template <typename T>
void AbstractProblem<T>::get_variables(const vector<pair<std::string, vector<int>>> &variables) {
    for (size_t i = 0; i < variables.size(); ++i) {
        this->ordered_variables.push_back(variables[i].first);
        this->variables_domain[variables[i].first] = variables[i].second;
        this->name_to_index[variables[i].first] = static_cast<int>(i);
        this->index_to_name[static_cast<int>(i)] = variables[i].first;
    }
    this->dd_to_original_index.resize(variables.size());
    iota(this->dd_to_original_index.begin(), this->dd_to_original_index.end(), 0);
}

template <typename T>
bool AbstractProblem<T>::transition_function(const T* previous_state, const int variable_index, int variable_value, T* scratch_state) const {
    throw NotImplementedError("Transition_function");
}

template <typename T>
int AbstractProblem<T>::get_priority_for_discard_node(const T* state) const {
    throw NotImplementedError("Get_priority_for_discard_node");
}

template <typename T>
int AbstractProblem<T>::get_priority_for_merge_nodes(const int node_id, const T* state) const {
    throw NotImplementedError("Get_priority_for_merge_nodes");
}

template <typename T>
T* AbstractProblem<T>::merge_operator(const T*state_one, const T* state_two) const {
    throw NotImplementedError("Merge_operator");
}

template <typename T>
string AbstractProblem<T>::get_state_as_string(const T* state) const {
    throw NotImplementedError("Get_state_as_string");
}

template <typename T>
T* AbstractProblem<T>::get_state_copy(const T* state) const {
    throw NotImplementedError("Get_state_copy");
}

template <typename T>
T* AbstractProblem<T>::get_final_state() {
    return this->initial_state;
}

template <typename T>
void AbstractProblem<T>::apply_variable_order(const vector<string>& new_order) {
    ordered_variables = new_order;
    name_to_index.clear();
    index_to_name.clear();
    for (size_t i = 0; i < ordered_variables.size(); ++i) {
        name_to_index[ordered_variables[i]] = static_cast<int>(i);
        index_to_name[static_cast<int>(i)] = ordered_variables[i];
    }
}

template <typename T>
vector<string> AbstractProblem<T>::sort_variables(const vector<pair<string, vector<int>>>& variables) {
    vector<string> names;
    names.reserve(variables.size());
    for (const auto& [name, domain] : variables)
        names.push_back(name);
    return names;
}


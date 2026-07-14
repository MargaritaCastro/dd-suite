
#include "AbstractDDBuilder.h"

template <typename T>
AbstractDDBuilder<T>::AbstractDDBuilder(AbstractProblem<T>& problem) :
        problem(problem),
        node_number(1),
        variables(problem.ordered_variables),
        graph(),
        scratch_state(problem.get_state_copy(problem.initial_state))
{
    set_variables_domains(problem.variables_domain);

    Node<T>* node_root = new Node(0, problem.get_state_copy(problem.initial_state));
    graph = new Graph(node_root);
    graph->structure.reserve(variables.size());
}

template <typename T>
AbstractDDBuilder<T>::~AbstractDDBuilder() {
    delete scratch_state;
}

template <typename T>
void AbstractDDBuilder<T>::set_variables_domains(map<string, vector<int>>& variables_domain_map){

    variables_domain.reserve(variables.size());

    for (size_t variable_id = 0; variable_id < variables.size(); ++variable_id) {
        variables_domain.push_back(variables_domain_map[variables[variable_id]]);
    }

}

template <typename T>
Graph<T>* AbstractDDBuilder<T>::get_decision_diagram(bool should_visualize) {

    for (size_t variable_id = 0; variable_id < variables.size(); ++variable_id) {
        create_new_layer(variable_id);
        specific_end_of_layer_function();
        print_graph(should_visualize);
    }

    specific_end_of_construction_function();
    set_final_state();
    print_graph(should_visualize);

    return graph;
}

template <typename T>
void AbstractDDBuilder<T>::create_new_layer(int variable_id) {
    graph->add_new_layer();

    create_new_nodes_in_the_new_layer(variable_id);
}

template <typename T>
void AbstractDDBuilder<T>::create_new_nodes_in_the_new_layer(int variable_id) {
    size_t last_layer_index = graph->structure.size() - 2;

    map_of_states.clear();
    map_of_states.reserve(graph->structure[last_layer_index].size() * variables_domain[variable_id].size());

    const string& variable_name = variables[variable_id];
    int variable_index = problem.name_to_index.at(variable_name);
    is_last_layer = (size_t)variable_id == variables.size() - 1;

    for (auto* parent_node : graph->structure[last_layer_index]) {
        for (int variable_value : variables_domain[variable_id]) {

            bool feasible = problem.transition_function(parent_node->state, variable_index, variable_value, scratch_state);
            if (!feasible) continue;

            if (is_last_layer && !graph->structure.back().empty()) {
                create_arcs_for_the_terminal_node(variable_value, parent_node);
                continue;
            }

            string key = problem.get_state_as_string(scratch_state);
            auto [it, inserted] = map_of_states.try_emplace(key, nullptr);
            if (inserted) {
                T* node_state = problem.get_state_copy(scratch_state);
                Node<T>* new_node = graph->create_and_add_node(node_number++, node_state);
                it->second = new_node;
                create_arc_for_the_new_node(parent_node, new_node, variable_value);
            } else {
                create_arc_for_the_new_node(parent_node, it->second, variable_value);
            }
        }
    }
}


template <typename T>
void AbstractDDBuilder<T>::create_arcs_for_the_terminal_node(int variable_value, Node<T> *parent_node) {
    Node<T>* same_state_node = graph->structure.back().back();
    create_arc_for_the_new_node(parent_node, same_state_node, variable_value);
}

template <typename T>
void AbstractDDBuilder<T>::create_arc_for_the_new_node(Node<T>* parent_node, Node<T>* node_created, int variable_value) {
    Arc<T>* arc = new Arc(parent_node, node_created, variable_value);

    parent_node->out_arcs.push_back(arc);
    node_created->in_arcs.push_back(arc);
}

template <typename T>
bool AbstractDDBuilder<T>::bottom_up_pruner() {
    bool delete_some_node = false;
    bool delete_some_node_layer = false;

    for (int layer_id = graph->structure.size() - 2; layer_id > 0; --layer_id) {
        delete_arcs_previous_layer(layer_id);
        delete_some_node_layer = delete_nodes_current_layer(layer_id);

        if (delete_some_node_layer) {
            delete_some_node = true;
        }
    }

    return delete_some_node;
}

template <typename T>
bool AbstractDDBuilder<T>::delete_nodes_current_layer(int layer_id){

    auto& layer = this->graph->structure[layer_id];
    bool delete_some_node = false;

    list<Node<T>*> live_nodes;

    for (auto* node : layer) {
        if (node->in_arcs.empty()) {
            delete node;
            delete_some_node = true;
        } else {
            live_nodes.push_back(node);
        }
    }

    if (delete_some_node) {
        layer = std::move(live_nodes);
    }

    return delete_some_node;
}

template <typename T>
void AbstractDDBuilder<T>::delete_arcs_previous_layer(int layer_id) {

    layer_id --;
    if (layer_id >= this->graph->structure.size() or layer_id < 0) {
        return;
    }

    Arc<T>* outgoing_arc = nullptr;

    for (auto& node : this->graph->structure[layer_id]) {
        for (auto it_arc = node->out_arcs.begin(); it_arc != node->out_arcs.end(); ) {
            outgoing_arc = *it_arc;
            if ((*it_arc)->child_node->out_arcs.empty()) {
                (*it_arc)->child_node->in_arcs.clear();
                it_arc = node->out_arcs.erase(it_arc);
                delete outgoing_arc;
            } else {
                ++it_arc;
            }
        }
    }

}


template <typename T>
void AbstractDDBuilder<T>::set_final_state() {
    Node<T>* last_node = graph->structure.back().back();
    if (last_node->state != nullptr) {
        delete last_node->state;
    }
    last_node->state = problem.get_final_state();
}

template <typename T>
void AbstractDDBuilder<T>::print_graph(bool should_visualize) {
    if (should_visualize) {
        print();
    }
}

template <typename T>
void AbstractDDBuilder<T>::print() {

    cout << "" << endl;
    for (size_t layer_idx = 0; layer_idx < graph->structure.size(); ++layer_idx) {
        cout << "------------------------------------------------------" << endl;
        if (layer_idx > 0 && layer_idx <= variables.size()) {
            cout << "[" << variables[layer_idx - 1] << "]" << "   ";
        }
        for (auto& node : graph->structure[layer_idx]) {
            string in_arcs_str;
            bool first = true;
            for (const auto& arc : node->in_arcs) {
                if (!first) in_arcs_str += ", ";
                in_arcs_str += arc->to_string();
                first = false;
            }
            cout << "u" << node->to_string() << " " << problem.get_state_as_string(node->state) << "(" << in_arcs_str << ")" << "   ";
        }
        cout << "" << endl;
    }
}

template <typename T>
void AbstractDDBuilder<T>::adjust_node_number() {
    int initial_node_number = 0;
    for (auto& layer : this->graph->structure) {
        for (auto& node : layer) {
            node->set_id(initial_node_number);
            initial_node_number++;
        }
    }
}
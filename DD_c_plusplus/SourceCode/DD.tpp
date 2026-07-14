#include "DD.h"
#include <utility>


template <typename T>
DD<T>::DD(AbstractProblem<T>& problem) :
    problem(problem),
    graph_DD()
{}

template <typename T>
DD<T>::~DD() {
    delete graph_DD;
}

template <typename T>
void DD<T>::create_decision_diagram(const bool verbose) {

    if (graph_DD != nullptr) {throw GraphAlreadyExists();}

    cout << endl;
    cout << "Starting the creation of the decision diagram..." << endl;
    auto start = chrono::steady_clock::now();
    ExactDDBuilder dDBuilder(problem);
    graph_DD = dDBuilder.get_decision_diagram(verbose);

    auto end = chrono::steady_clock::now();

    dd_builder_time = chrono::duration<double>(end - start);
    cout << "Decision diagram successfully created." << endl;

}

template <typename T>
void DD<T>::create_restricted_decision_diagram(int max_width, const bool verbose) {

    if (graph_DD != nullptr) {throw GraphAlreadyExists();}

    cout << endl;
    cout << "Starting the creation of the restricted decision diagram..." << endl;
    auto start = chrono::steady_clock::now();

    RestrictedDDBuilder restrictedDDBuilder(problem, max_width);
    graph_DD = restrictedDDBuilder.get_decision_diagram(verbose);
    auto end = chrono::steady_clock::now();

    dd_builder_time = chrono::duration<double>(end - start);
    cout << "Restricted decision diagram creation completed." << endl;
}

template <typename T>
void DD<T>::create_relax_priority_decision_diagram(int max_width, const bool verbose) {

    if (graph_DD != nullptr) {throw GraphAlreadyExists();}

    cout << endl;
    cout << "Starting the creation of the relax priority decision diagram..." << endl;
    auto start = chrono::steady_clock::now();

    RelaxedPriorityDDBuilder relaxedDDBuilder(problem, max_width);
    graph_DD = relaxedDDBuilder.get_decision_diagram(verbose);

    auto end = chrono::steady_clock::now();
    cout << "Relax priority decision diagram creation completed." << endl;

    dd_builder_time = chrono::duration<double>(end - start);
}

template <typename T>
void DD<T>::create_relax_grouping_decision_diagram(int max_width, const bool verbose) {

    if (graph_DD != nullptr) {throw GraphAlreadyExists();}

    cout << endl;
    cout << "Starting the creation of the relax grouping decision diagram..." << endl;
    auto start = chrono::steady_clock::now();

    RelaxedGroupingDDBuilder relaxedGroupingDDBuilder(problem, max_width);
    graph_DD = relaxedGroupingDDBuilder.get_decision_diagram(verbose);

    auto end = chrono::steady_clock::now();
    cout << "Relax grouping decision diagram creation completed." << endl;

    dd_builder_time = chrono::duration<double>(end - start);
}

template <typename T>
void DD<T>::reduce_decision_diagram(const bool verbose) {
    if (graph_DD == nullptr) {throw NoGraphFound();}

    cout << endl << "Starting the reduction of the decision diagram..." << endl;
    auto start = chrono::steady_clock::now();

    DDReducer reduceDDBuilder(graph_DD, problem);
    reduceDDBuilder.reduce_decision_diagram(verbose);

    auto end = chrono::steady_clock::now();
    cout << "Reduced decision diagram creation completed." << endl;

    dd_reduction_time = chrono::duration<double>(end - start);
}

template <typename T>
string DD<T>::get_building_time() {
    return to_string(dd_builder_time.count());
}

template <typename T>
string DD<T>::get_reduction_time() {
    return to_string(dd_reduction_time.count());
}

template <typename T>
Graph<T>* DD<T>::get_decision_diagram() {
    if (!graph_DD || graph_DD->structure.empty()) {throw NoGraphFound();}
    return graph_DD;
}

template <typename T>
Graph<T>* DD<T>::get_decision_diagram_copy() {
    if (!graph_DD || graph_DD->structure.empty())  {throw NoGraphFound();}

    unordered_map<string, Node<T>*> nodes_map;

    Node<T>* root_node = new Node<T>(0, problem.get_state_copy(graph_DD->structure[0].front()->state));
    nodes_map[graph_DD->structure[0].front()->to_string()] = root_node;
    Graph<T>* graphCopy= new Graph(root_node);

    // Iterate over each layer of `graph_DD`
    for (int layer = 1; layer < graph_DD->structure.size(); ++layer) {
        graphCopy->add_new_layer();

        // Iterate over each node of the layer
        for (auto& node : graph_DD->structure[layer]) {
            Node<T>* node_copy = new Node<T>(node->get_id(), problem.get_state_copy(node->state));
            nodes_map[node->to_string()] = node_copy;

            // Copy input arcs
            for (auto& in_arc : node->in_arcs) {
                auto parent = nodes_map[in_arc->parent_node->to_string()];
                Arc<T>* arc = new Arc<T>(parent, node_copy, in_arc->variable_value);
                parent->out_arcs.push_back(arc);
                node_copy->in_arcs.push_back(arc);
            }

            // Add the copied node to the new graph
            graphCopy->add_node(node_copy);
        }
    }

    return graphCopy;
}

template <typename T>
void DD<T>::export_graph_file(string file_name) {
    if (graph_DD == nullptr) {throw NoGraphFound();}

    GraphFile graphFile(std::move(file_name), graph_DD, problem);
}

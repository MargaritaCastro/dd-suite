#include "GraphAlgorithms/MaxFlow/MaxFlowGurobi.h"

template <typename T>
MaxFlowGurobi<T>::MaxFlowGurobi(DD<T>* DD) :
 graph_DD(DD->get_decision_diagram()), time(0), max_flow_computed(false), model(nullptr) {
    init_arc_ids();
}

template <typename T>
MaxFlowGurobi<T>::~MaxFlowGurobi() {
    graph_DD = nullptr;
    if (model != nullptr) {
        delete model;
        model = nullptr;
    }
}

template <typename T>
void MaxFlowGurobi<T>::init_arc_ids() {
    int arc_id = 0;
    for (const auto& layer : graph_DD->structure)
        for (Node<T>* node : layer)
            for (Arc<T>* arc : node->out_arcs) {
                arc->arc_id = arc_id;
                arc_list.push_back(arc);
                arc_id += 2;
            }
    capacities.resize(arc_list.size(), 0.0);
    flows.resize(arc_list.size());
}

template <typename T>
double MaxFlowGurobi<T>::get_time() const {
    return time;
}

template <typename T>
double MaxFlowGurobi<T>::get_max_flow() {
    if (!max_flow_computed) {
        throw MaxFlowNotExecuted();
    }
    return max_flow;
}

template <typename T>
double MaxFlowGurobi<T>::solve_max_flow(const vector<double>& new_capacities) {
    set_capacities(new_capacities);
    auto start = chrono::high_resolution_clock::now();
    max_flow = compute_max_flow();
    max_flow_computed = true;
    auto end = chrono::high_resolution_clock::now();
    time = chrono::duration<double>(end - start).count();
    cout << "\n'''The maximum flow in the graph is " << max_flow << "''' \n" << endl;
    return max_flow;
}

template <typename T>
void MaxFlowGurobi<T>::set_capacities(const vector<double>& new_caps) {
    for (int layer = 0; layer < (int)graph_DD->structure.size(); ++layer) {
        double cap = (layer < (int)new_caps.size()) ? new_caps[layer] : 0.0;
        for (Node<T>* node : graph_DD->structure[layer])
            for (Arc<T>* arc : node->out_arcs)
                capacities[arc->arc_id >> 1] = (arc->variable_value == 1) ? cap : 1.0 - cap;
    }
}

template <typename T>
double MaxFlowGurobi<T>::compute_max_flow() {
    if (model != nullptr) {
        delete model;
    }
    model = new GRBModel(GRBEnv());
    initialize_variables();
    add_constraints();
    set_objective();
    optimize();
    return model->get(GRB_DoubleAttr_ObjVal);
}

template <typename T>
void MaxFlowGurobi<T>::initialize_variables() {
    flows.resize(arc_list.size());
    for (Arc<T>* arc : arc_list) {
        int idx = arc->arc_id >> 1;
        flows[idx] = model->addVar(
            0, capacities[idx], 0, GRB_CONTINUOUS,
            "flow_" + to_string(arc->parent_node->get_id()) + "_" + to_string(arc->child_node->get_id())
        );
    }
}

template <typename T>
void MaxFlowGurobi<T>::add_constraints() {
    /*
     * O(E) instead of the previous O(V^2): iterate in_arcs/out_arcs directly
     * instead of searching over all (v, node, arc_value) triplets.
     */
    int source_id = graph_DD->structure[0].front()->get_id();
    int sink_id = graph_DD->structure.back().front()->get_id();

    for (const auto& layer : graph_DD->structure) {
        for (Node<T>* node : layer) {
            int nid = node->get_id();
            if (nid == source_id || nid == sink_id) continue;

            GRBLinExpr inflow = 0, outflow = 0;
            for (Arc<T>* arc : node->in_arcs)
                inflow += flows[arc->arc_id >> 1];
            for (Arc<T>* arc : node->out_arcs)
                outflow += flows[arc->arc_id >> 1];
            model->addConstr(inflow == outflow, "flow_conservation_" + to_string(nid));
        }
    }
}

template <typename T>
void MaxFlowGurobi<T>::set_objective() {
    Node<T>* source = graph_DD->structure[0].front();
    GRBLinExpr outflow = 0;
    for (Arc<T>* arc : source->out_arcs)
        outflow += flows[arc->arc_id >> 1];
    model->setObjective(outflow, GRB_MAXIMIZE);
}

template <typename T>
void MaxFlowGurobi<T>::optimize() {
    model->optimize();
    show_results();
}

template <typename T>
void MaxFlowGurobi<T>::show_results() const {
    if (model->get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
        cout << "Maximum flow: " << model->get(GRB_DoubleAttr_ObjVal) << endl;
        for (Arc<T>* arc : arc_list) {
            double x = flows[arc->arc_id >> 1].get(GRB_DoubleAttr_X);
            if (x > 0)
                cout << "Flow in " << arc->parent_node->get_id() << " -> "
                     << arc->child_node->get_id() << " (" << arc->variable_value << "): "
                     << round(x * 1000) / 1000 << endl;
        }
    } else {
        cout << "No optimal solution found." << endl;
    }
}

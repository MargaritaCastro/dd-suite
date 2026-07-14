#include "GraphFile.h"

template <typename T>
GraphFile<T>::GraphFile(const string& file_name, Graph<T>* graph, AbstractProblem<T>& problem) :
    file_name(file_name),
    graph(graph),
    problem(problem)
    {

    string source_directory = fs::current_path().parent_path().string();
    string full_file_path = source_directory + "/" + file_name + ".gml";

    bdd_file = new ofstream(full_file_path);
    is_graph_binary = check_if_graph_is_binary();
    create_gml_file();
}

template <typename T>
GraphFile<T>::~GraphFile() {

    if (bdd_file != nullptr) {
        delete bdd_file;
        bdd_file = nullptr;
    }
}

template <typename T>
bool GraphFile<T>::check_if_graph_is_binary() {
    for (auto& layer: graph->structure) {
        for (auto& node: layer) {
            if (!is_node_binary(node)) {
                return false;
            }
        }
    }
    return true;
}

template <typename T>
bool GraphFile<T>::is_node_binary(Node<T>* node) {
    for (auto& arc: node->out_arcs) {
        if (arc->variable_value != 0 && arc->variable_value != 1) {
            return false;
        }
    }

    return true;
}

template <typename T>
void GraphFile<T>::create_gml_file() {
    start_file();
    add_nodes_and_arcs();
    end_file();
}

template <typename T>
void GraphFile<T>::start_file() {
    (*bdd_file) << "graph [\n";
    (*bdd_file) << "\t directed 1\n";
    (*bdd_file) << "\t hierarchic 1\n";
}

template <typename T>
void GraphFile<T>::add_nodes_and_arcs() {
    vector<Arc<T>*> arcs = {};
    for (auto& layer: graph->structure) {
        for (auto& node: layer) {
            add_node(node);
            for (auto& arc: node->out_arcs) {
                arcs.push_back(arc);
            }
        }
    }
    add_arcs(arcs);
}

template <typename T>
void GraphFile<T>::add_node(Node<T>* node) {
    (*bdd_file) << "\n node [\n";
    (*bdd_file) << "\t id " << node->get_id() << "\n";
    (*bdd_file) << "\t label \"" << node->get_id() << "             " << problem.get_state_as_string(node->state) << "\"\n";
    (*bdd_file) << " \tgraphics [\n";
    (*bdd_file) << "\t type \"ellipse\"\n";
    (*bdd_file) << "\t hasFill 0\n";
    (*bdd_file) << "\t w 90.0   h 110.0\n";
    (*bdd_file) << "\t outline \"#000000\" ]\n";
    (*bdd_file) << "]\n";
    (*bdd_file) << " \tLabelGraphics [\n";
    (*bdd_file) << "\t text \"" << node->get_id() << "             "  << problem.get_state_as_string(node->state) << "\"\n";
    (*bdd_file) << "\t fontSize 12\n";
    (*bdd_file) << "\t fontName \"Dialog\"\n";
    (*bdd_file) << "\t model \"sides\"\n";
    (*bdd_file) << "\t anchor \"e\"\n";
    (*bdd_file) << "\t borderDistance -50.0\n";
    (*bdd_file) << "]\n";
}

template <typename T>
void GraphFile<T>::add_arcs(const vector<Arc<T>*>& arcs) {
    for (auto& arc: arcs) {
        add_arc(arc);
    }
}

template <typename T>
void GraphFile<T>::add_arc(Arc<T>* arc) {
    (*bdd_file) << "\nedge [\n";
    add_arc_source(arc);
    add_arc_target(arc);

    if (not is_graph_binary) {
        add_arc_label(arc);
    }

    add_arc_graphics(arc);
    (*bdd_file) <<"]\n";
}

template <typename T>
void GraphFile<T>::add_arc_source(Arc<T>* arc) {
    (*bdd_file) << "\t source " << arc->parent_node->get_id() << "\n";
}

template <typename T>
void GraphFile<T>::add_arc_label(Arc<T>* arc) {
    (*bdd_file) << "\tlabel \"" << arc->variable_value << "\"\n";
}

template <typename T>
void GraphFile<T>::add_arc_target(Arc<T>* arc) {
    (*bdd_file) << "\ttarget " << arc->child_node->get_id() << "\n";
}

template <typename T>
void GraphFile<T>::add_arc_graphics(Arc<T>* arc) {
    (*bdd_file) << "\tgraphics [\n";

    if (is_graph_binary) {
        add_binary_arc_graphics(arc);
    } else {
        add_normal_arc_graphics(arc);
    }
    (*bdd_file) << "]";
}

template <typename T>
void GraphFile<T>::add_normal_arc_graphics(Arc<T>* arc) {
    (*bdd_file) << "\tfill \"#000000\" 		targetArrow \"standard\"	 	 \n";
}

template <typename T>
void GraphFile<T>::add_binary_arc_graphics(Arc<T>* arc) {
    if (arc->variable_value == 0) {
        (*bdd_file) << "\tfill \"#808080\" 		targetArrow \"standard\"	  style	  \"dashed\"	\n";
    } else {
        (*bdd_file) << "\tfill \"#000000\" 		targetArrow \"standard\"	 	 \n";
    }
}

template <typename T>
void GraphFile<T>::end_file() {
    (*bdd_file) << "]\n";
    bdd_file->close();
}

#ifndef TEMPLATETYPE_GRAPHFILE_H
#define TEMPLATETYPE_GRAPHFILE_H

#include <iostream>
#include <string>
#include <vector>

#include "Graph.h"

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

using namespace std;

/**
 * @brief Class that represents a graph file.
 * @tparam T Type of the Node state values
 */
template <typename T>
class GraphFile {
public:
    /**
     * @brief Constructor for the GraphFile class.
     * @param file_name Name of the graph file.
     * @param graph Graph that will be written to the file.
     * @param problem Problem associated with the graph.
     */
    GraphFile(const string& file_name, Graph<T>* graph, AbstractProblem<T>& problem);

    /**
     * @brief Default constructor for the GraphFile class.
     */
    GraphFile() = default;

    /**
     * @brief Default destructor for the GraphFile class.
     */
    ~GraphFile();

private:
    string file_name; /**< Name of the graph file. */
    Graph<T>* graph; /**< Graph that will be written to the file. */
    bool is_graph_binary; /**< Indicates whether the graph is binary. */
    ofstream* bdd_file; /**< Pointer to the graph file. */
    AbstractProblem<T>& problem; /**< Reference to the problem associated with the graph. */

    /**
     * @brief Creates the graph file.
     */
    void create_gml_file();
    /**
     * @brief Initializes the structure of the GML file with graph information.
     */
    void start_file();
    /**
     * @brief Adds nodes to the GML file, along with their outgoing arc information.
     */
    void add_nodes_and_arcs();
    /**
     * @brief Determines if a graph is binary, i.e., if all its variables are binary.
     * @return bool indicating whether the graph is binary.
     */
    bool check_if_graph_is_binary();
    /**
     * @brief Determines if a node is binary, i.e., if its outgoing arc have values of 0 or 1.
     * @param node Node object that has arc with variable values.
     * @return bool indicating whether the node is binary.
     */
    bool is_node_binary(Node<T>* node);
    /**
     * @brief Adds node information to the GML file.
     * @param node Node object to add to the GML file.
     */
    void add_node(Node<T>* node);
    /**
     * @brief Adds arc to the GML file.
     * @param vector<Arc<T>*> arc Vector of arc to add to the GML file.
     */
    void add_arcs(const vector<Arc<T>*>& arcs);
    /**
     * @brief Adds arc information to the GML file.
     * @param arc Arc object to add to the GML file.
     */
    void add_arc(Arc<T>* arc);
    /**
     * @brief Adds source information to the GML file.
     * @param arc Arc object to add to the GML file.
     */
    void add_arc_source(Arc<T>* arc);
    /**
     * @brief Adds label information to the GML file.
     * @param arc Arc object to add to the GML file.
     */
    void add_arc_label(Arc<T>* arc);
    /**
     * @brief Adds target information to the GML file.
     * @param arc Arc object to add to the GML file.
     */
    void add_arc_target(Arc<T>* arc);
    /**
     * @brief Adds graphics information to the GML file.
     * @param arc Arc object to add to the GML file.
     */
    void add_arc_graphics(Arc<T>* arc);
    /**
     * @brief Adds normal arc graphics information to the GML file.
     * @param arc Arc object to add to the GML file.
     */
    void add_normal_arc_graphics(Arc<T>* arc);
    /**
     * @brief Adds binary arc graphics information to the GML file.
     * @param arc Arc object to add to the GML file.
     */
    void add_binary_arc_graphics(Arc<T>* arc);
    /**
     * @brief Finalizes the structure of the GML file.
     */
    void end_file();
};
#include "GraphFile.tpp"
#endif //TEMPLATETYPE_GRAPHFILE_H

#ifndef TEMPLATETYPE_DD_H
#define TEMPLATETYPE_DD_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cassert>
#include <set>
#include <chrono>
#include <memory>
#include <thread>

#include "AbstractProblemClass.h"
#include "Graph.h"
#include "GraphFile.h"
#include "DDBuilder/ExactDDBuilder/ExactDDBuilder.h"
#include "RestrictedDDBuilder.h"
#include "RelaxedPriorityDDBuilder.h"
#include "RelaxedGroupingDDBuilder.h"
#include "DDReducer.h"

using namespace std;

/**
 * @brief Which builder produced a decision diagram.
 */
enum class DDKind { Exact, Restricted, Relaxed };

/**
 * @brief Class representing a decision diagram controller associated with a problem (DD).
 * @tparam T Type of the Node state values.
 */
template <typename T>
class DD {
public:
    /**
     * @brief Constructor for the DD class.
     * @param problem Reference to an AbstractProblem object representing the problem to be solved.
     * @param verbose Boolean indicating if the decision diagram construction process should be visualized.
     */
    DD(AbstractProblem<T>& problem);


    AbstractProblem<T>& problem; /**< Reference to the problem to be solved. */

    /**
     * @brief Creates the decision diagram.
     * @param verbose Boolean indicating if progress messages should be displayed during the diagram construction.
     */
    void create_decision_diagram(const bool verbose = false);

    /**
     * @brief Reduced decision diagram and stores it in the graph_DD attribute.
     * @param verbose Boolean indicating if progress messages should be displayed during the diagram construction.
     */
    void reduce_decision_diagram(const bool verbose = false);

    /**
     * @brief Creates a restricted decision diagram and stores it in the graph_DD attribute.
     * @param verbose Boolean indicating if progress messages should be displayed during the diagram construction.
     */
    void create_restricted_decision_diagram(int max_width, const bool verbose = false);

    /**
     * @brief Creates a relaxed decision diagram (merging nodes by priority) and stores it in the graph_DD attribute.
     * @param verbose Boolean indicating if progress messages should be displayed during the diagram construction.
     */
    void create_relax_priority_decision_diagram(int max_width, const bool verbose = false);

    /**
     * @brief Creates a relaxed decision diagram (merging nodes in groups by priority difference)
     * and stores it in the graph_DD attribute.
     * @param verbose Boolean indicating if progress messages should be displayed during the diagram construction.
     */
    void create_relax_grouping_decision_diagram(int max_width, const bool verbose = false);

    /**
     * @brief Exports the decision diagram graph to a file.
     * @param file_name Name of the file.
     */
    void export_graph_file(string file_name);

    /**
     * @brief Gets a reference to the decision diagram graph.
     * @return Reference to the decision diagram graph.
     */
    Graph<T>* get_decision_diagram();

    /**
     * @brief Gets a unique copy of the decision diagram graph.
     * @return Unique pointer to the decision diagram graph.
     */
    Graph<T>* get_decision_diagram_copy();

    // Methods to get the construction time of the decision diagrams.
    string get_building_time();
    string get_reduction_time();

    /**
     * @brief Gets which builder produced the diagram.
     * @return Exact, Restricted or Relaxed; Exact until a diagram is built.
     */
    DDKind get_dd_kind() const;

    /**
     * @brief Destructor for the DD class.
     */
    ~DD();

private:
    Graph<T>* graph_DD; /**< Decision diagram graph. */
    DDKind dd_kind = DDKind::Exact; /**< Builder that produced the graph. */

    // Attributes that store the construction time of the decision diagrams.
    chrono::duration<double>  dd_builder_time = chrono::duration<double>(0);
    chrono::duration<double>  dd_reduction_time = chrono::duration<double>(0);

};

#include "DD.tpp"
#endif //TEMPLATETYPE_DD_H

#ifndef TEMPLATETYPE_RESTRICTEDDDBUILDER_H
#define TEMPLATETYPE_RESTRICTEDDDBUILDER_H

#include "AbstractDDBuilder.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

/**
 * @brief Structure representing a comparator for deleting nodes.
 * @tparam T Data type of the nodes state.
 */
template <typename T>
struct NodeDeleteComparator {
    AbstractProblem<T>& problem;
    NodeDeleteComparator(AbstractProblem<T>& problem) : problem(problem) {}
    bool operator()(const Node<T>* node_one, const Node<T>* node_two) const;
};


/**
 * @brief Class representing a builder for a restricted decision diagram.
 * @tparam T Data type of the nodes state.
 */
template <typename T>
class RestrictedDDBuilder: public AbstractDDBuilder<T> {
public:
    /**
     * @brief Constructor for the RestrictedDDBuilder class.
     * @param problem Reference to an AbstractProblem object representing the problem to be solved.
     * @param max_width Maximum allowed width for the decision diagram.
     */
    RestrictedDDBuilder(AbstractProblem<T>& problem, int max_width);

private:
    int max_width; /**< Maximum allowed width for the decision diagram. */
    multiset<Node<T>*, NodeDeleteComparator<T>> nodes_set; /**< Set of nodes ordered by priority. */

    /**
     * @brief Specific function for the layer of the graph in the construction of a restricted decision diagram.
     * Implements the mandatory function from the parent class.
     */
    void specific_end_of_layer_function() override;

    /**
     * @brief Specific function for the final layer of the graph in the construction of a restricted decision diagram.
     * Implements the mandatory function from the parent class.
     */
    void specific_end_of_construction_function() override;

    /**
     * @brief Eliminates nodes from the decision diagram when the width is greater than max_width.
     */
    void eliminate_nodes_when_width_is_greater_than_w();

    /**
    * @brief Eliminates nodes from a specific layer fo de DD.
    * @param nodes_to_eliminate Vector of pointers to the nodes to be eliminated.
    */
    void eliminate_nodes_from_layer(const list<Node<T>*>& nodes_to_eliminate, int layer_id);

    /**
     * @brief Remove outgoing arcs of previous layer after discarding nodes in current layer (layer_id)
     * @param layer_id id of the layer
     */
    void remove_outgoing_arcs_previous_layer(int layer_id);

};
#include "RestrictedDDBuilder.tpp"
#endif //TEMPLATETYPE_RESTRICTEDDDBUILDER_H

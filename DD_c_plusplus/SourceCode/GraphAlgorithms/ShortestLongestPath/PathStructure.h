#ifndef TEMPLATETYPE_OBJECTIVESTRUCTURE_H
#define TEMPLATETYPE_OBJECTIVESTRUCTURE_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <tuple>

#include "Graph.h"

using namespace std;

/**
 * @brief Structure to store information while solving the ShortestLongestPath algorithm.
 * @tparam T Type of the Node state values
 */
template <typename T>
struct PathStructure {
    double value; /**< Value of the path. */
    Arc<T>* arc; /**< Arc object in the same order as the path_print. */

    /**
     * @brief Default constructor. Initializes default values (-111111, "", {}).
     */
    PathStructure();

    /**
     * @brief Constructor with parameters.
     * @param value Value of the path.
     * @param arc Arc object pointing to the associated node.
     */
    PathStructure(double value, Arc<T>* arc);
};

/**
 * @brief Structure that stores the solution of the ShortestLongestPath algorithm.
 * @tparam T Type of the Node state values
 */
template <typename T>
struct PathStructureSolution {
    double value; /**< Value of the path. */
    string path_print; /**< Path in string format explaining the route that results in the value. */
    list<Arc<T>* > path_arcs; /**< Path of arcs that results in the value. */

    /**
     * @brief Default constructor. Initializes default values (-111111, "", {}).
     */
    PathStructureSolution();

    /**
     * @brief Constructor with parameters.
     * @param value Value of the path.
     * @param path Path in string format explaining the route that results in the value.
     * @param arcs Arc objects in the same order as the path_print.
     */
    PathStructureSolution(double value, string path, list<Arc<T>*> arcs);
};

#include "PathStructure.tpp"
#endif //TEMPLATETYPE_OBJECTIVESTRUCTURE_H

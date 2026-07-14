#ifndef TEMPLATETYPE_DDBUILDER_H
#define TEMPLATETYPE_DDBUILDER_H


#include "AbstractDDBuilder.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * @brief Concrete class that inherits from AbstractDDBuilder, generating the basic construction of a decision diagram.
 * @tparam T Type of the Node state values
 */
template <typename T>
class ExactDDBuilder: public AbstractDDBuilder<T> {
public:
    /**
     * @brief Constructor for the ExactDDBuilder class.
     * @param problem Optimization problem to be solved.
     */
    ExactDDBuilder(AbstractProblem<T>& problem);
private:
    /**
     * @brief Specific layer function for the construction of decision diagrams.
     *        Not implemented
     */
    void specific_end_of_layer_function() override;

    /**
     * @brief Specific final function for the construction of decision diagrams.
     *        Not implemented
     */
    void specific_end_of_construction_function() override;
};

#include "ExactDDBuilder.tpp"
#endif //TEMPLATETYPE_DDBUILDER_H

#include "DDBuilder/ExactDDBuilder/ExactDDBuilder.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

using namespace std;

template <typename T>
ExactDDBuilder<T>::ExactDDBuilder(AbstractProblem<T>& problem) : AbstractDDBuilder<T>(problem) {}

template <typename T>
void ExactDDBuilder<T>::specific_end_of_layer_function() {

}

template <typename T>
void ExactDDBuilder<T>::specific_end_of_construction_function() {
    bool delete_some_node;
    delete_some_node = this->bottom_up_pruner();

    if (delete_some_node) {
        this->adjust_node_number();
    }
}

#include "GraphAlgorithms/ShortestLongestPath/PathStructure.h"

template <typename T>
PathStructure<T>::PathStructure() : value(-111111), arc(nullptr) {}

template <typename T>
PathStructure<T>::PathStructure(double value, Arc<T>* arc)
    : value(value),  arc(arc) {}

template <typename T>
PathStructureSolution<T>::PathStructureSolution() : value(-111111), path_print(""), path_arcs({}) {}

template <typename T>
PathStructureSolution<T>::PathStructureSolution(double value, string path, list<Arc<T>*> arcs)
        : value(value), path_print(path), path_arcs(arcs) {}

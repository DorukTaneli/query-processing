#ifndef PARMAT_VECTOR_HPP
#define PARMAT_VECTOR_HPP

#include <vector>

#include "internal_config.hpp"
#include "utils.hpp"

std::vector<std::tuple<int, int, int>> generate_graph_in_vector(const unsigned long long nEdges,
                                                                unsigned long long nVertices);

#endif //      PARMAT_VECTOR_HPP

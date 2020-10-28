#ifndef GRAPH_GEN_NOT_SORTED_HPP
#define GRAPH_GEN_NOT_SORTED_HPP

#include <fstream>
#include <iostream>
#include <random>
#include <tuple>

namespace GraphGen_notSorted{

	bool GenerateGraph(
			const unsigned long long nEdges,
			const unsigned long long nVertices,
			const double a, const double b, const double c,
			const unsigned int nCPUWorkerThreads,
			std::ofstream& outFile,
			const unsigned long long standardCapacity,
			const bool allowEdgeToSelf,
			const bool allowDuplicateEdges,
			const bool directedGraph
			);

	std::vector<std::tuple<int, int, int>> GenerateGraph_in_vector(
                        const unsigned long long nEdges,
                        const unsigned long long nVertices,
                        const double a, const double b, const double c,
                        const unsigned int nCPUWorkerThreads,
                        const unsigned long long standardCapacity,
                        const bool allowEdgeToSelf,
                        const bool allowDuplicateEdges,
                        const bool directedGraph
                        );
};

#endif	//	GRAPH_GEN_NOT_SORTED_HPP

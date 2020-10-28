#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#include "GraphGen_notSorted.hpp"
#include "PaRMAT_vector.hpp"
#include "internal_config.hpp"
#include "utils.hpp"
#include <algorithm>

std::vector<std::tuple<int, int, int>> generate_graph_in_vector(const unsigned long long nEdges,
                                                                unsigned long long nVertices) {

  double a = 0.45, b = 0.22, c = 0.22;
  unsigned int nCPUWorkerThreads = 0;
  double RAM_usage = 0.5;
  bool allowEdgeToSelf = false;
  bool allowDuplicateEdges = false;
  bool directedGraph = true;
  unsigned long long standardCapacity = 0;

  // Avoiding very small regions which may cause incorrect results.
  if(nEdges < 10000)
    nCPUWorkerThreads = 1;
  else
    nCPUWorkerThreads = std::max(1, static_cast<int>(std::thread::hardware_concurrency()) - 1);

  auto totalSystemRAM = static_cast<unsigned long long>(getTotalSystemMemory()); // In bytes.
  auto availableSystemRAM = calculateAvailableRAM(totalSystemRAM, RAM_usage);    // In bytes.

  standardCapacity = availableSystemRAM /
                     (2 * nCPUWorkerThreads * sizeof(Edge)); // 2 can count for vector's effect.

  // Generate Graph
  --nVertices;
  std::vector<std::tuple<int, int, int>> graphVector = GraphGen_notSorted::GenerateGraph_in_vector(
      nEdges, nVertices, a, b, c, nCPUWorkerThreads, standardCapacity, allowEdgeToSelf,
      allowDuplicateEdges, directedGraph);

  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(graphVector.begin(), graphVector.end(), g);

  return graphVector;
}

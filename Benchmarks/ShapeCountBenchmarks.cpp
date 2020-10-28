#include "../Dependencies/PaRMAT_vector.hpp"
#include "../Source/ShapeCount.hpp"
#include <benchmark/benchmark.h>
#include <iostream>
#include <random>
#include <unordered_map>
using namespace std;

////////////////////////////////////////
#include "../Source/ShapeCount.c"

template <typename Implementation> static void GraphQueryBenchmark(benchmark::State& state) {
  Implementation const implementation;
  vector<tuple<int, int, int>> edges{};
  int const graphConnectedness = state.range(1);

  // Change number of edges and vetices
  unsigned long long nEdges = state.range(0) * graphConnectedness;
  unsigned long long nVertices = state.range(0);

  edges = generate_graph_in_vector(nEdges, nVertices);

  auto batch = nEdges / 4;
  for(auto _ : state) {
    auto* store = implementation.allocateDatabase(state.range(0) * graphConnectedness);
    for(auto i = 0U; i + batch < state.range(0) * graphConnectedness; i += batch) {
      for(auto j = 0u; j < batch; j++) {
        implementation.insertEdge(store, get<0>(edges[i + j]), get<1>(edges[i + j]),
                                  get<2>(edges[i + j]));
      }
      benchmark::DoNotOptimize(implementation.runQuery(store, 3, 1, 0));
      for(auto j = 0u; j < batch; j += 10) {
        implementation.deleteEdge(store, get<0>(edges[i + j]), get<1>(edges[i + j]),
                                  get<2>(edges[i + j]));
      }
    }
    implementation.deleteDatabase(store);
  }
}
BENCHMARK_TEMPLATE(GraphQueryBenchmark, HashjoinImplementation)
    ->RangeMultiplier(2)
    ->Ranges({{64, 2048}, {32, 32}});
BENCHMARK_TEMPLATE(GraphQueryBenchmark, SortMergeJoinImplementation)
    ->RangeMultiplier(2)
    ->Ranges({{64, 2048}, {32, 32}});
BENCHMARK_TEMPLATE(GraphQueryBenchmark, CompetitionImplementation)
    ->RangeMultiplier(2)
    ->Ranges({{512 * 1024, 512 * 1024 * 1024}, {32, 32}});

BENCHMARK_MAIN();

#pragma once

#include <flow/MinimumCostFlow.hpp>
#include <algorithm>
#include <vector>

namespace Koala {

class EdmondsKarpMCF final : public MinimumCostFlow {
  struct Edge {
      uint32_t from, to;
      int64_t cost, capacity, flow;
  }; 
  std::vector<Edge> edges;
  std::vector<std::vector<uint32_t>> neigh_list;
  std::vector<int64_t> b, excess;
  std::vector<int64_t> potential;
  std::unordered_map<NetworKit::Edge, std::int64_t> computed_flow;
  uint32_t n;

  void run_impl() override;
  void initialize();
  void delta_scaling_phase(uint64_t);
  void augmenting_phase(uint32_t, uint32_t, uint64_t);
  void send(std::uint64_t, std::int64_t);
  std::vector<std::pair<int64_t, uint64_t>> dijkstra(uint32_t source, int64_t delta);

 public: 
  EdmondsKarpMCF(MCFlowNetwork const& network) : MinimumCostFlow(network) {}
  int64_t getFlow(NetworKit::Edge const&) override;
};

} /* namespace Koala */
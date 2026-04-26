#pragma once

#include <flow/MinimumCostFlow.hpp>
#include <flow/PushRelabel.hpp>
#include <networkit/graph/Attributes.hpp>
#include <map>
#include <cstdint>
#include <stack>
#include <climits>
#include <vector>
#include <set>
#include <optional>

namespace Koala {

class OrlinMCF final : public MinimumCostFlow {
   struct Edge {
      uint32_t from, to;
      int64_t cost, capacity, flow;
   };

   NetworKit::Graph original_graph;
   std::vector<Edge> edges;
   std::vector<Edge> original_edges;
   std::vector<std::vector<uint32_t>> neigh_list;
   std::vector<int64_t> potential;
   // potential that is computed for the original network
   std::vector<int64_t> potential_computed;
   std::vector<int64_t> excess;
   uint32_t nodes_number;
   uint32_t max_nodeid;
   // Storing with costs for potential resotration  
   std::stack<std::tuple<uint32_t, uint32_t, int64_t>> contracted_nodes;
   const double ALPHA = 0.9;

   std::optional<Koala::PushRelabel> maxflow;
   void run_impl() override;
   bool is_imbalanced();
   void initialize();
   void find_optimal_delta(int64_t &delta);
   void push_no_excess(uint32_t edge_idx, int64_t amount); 
   void contract_nodes(uint32_t u, uint32_t v);
   void apply_potential();
   void contraction_phase(int64_t delta);
   void augmenting_phase(uint32_t s, uint32_t t, int64_t delta);
   void uncontract_nodes_potential();
   std::vector<std::pair<int64_t, uint64_t>> dijkstra(uint32_t source, int64_t delta);
   std::stack<std::pair<NetworKit::node, NetworKit::node>> contractions;
   std::pair<NetworKit::node, NetworKit::node> uncapacitated_nodes_bounds;
   bool is_added_uncapacitated(NetworKit::node v) const;
   void compute_final_flows();

 public:
    OrlinMCF(Koala::MCFlowNetwork& network) : MinimumCostFlow(network) {}
    int64_t getFlow(const NetworKit::Edge& edge) override; 

};

} /* namespace Koala */

 
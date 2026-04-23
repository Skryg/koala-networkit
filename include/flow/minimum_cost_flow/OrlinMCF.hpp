#pragma once

#include <flow/MinimumCostFlow.hpp>
#include <flow/MaximumFlow.hpp>
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
   
   uint32_t n, m;
   std::vector<int64_t> potential;
   std::vector<int64_t> excess;
   uint32_t nodes_number;
   // Storing with costs for potential resotration  
   std::stack<std::tuple<uint32_t, uint32_t, int64_t>> contracted_nodes;
   const double ALPHA = 0.9;

   std::optional<Koala::KingRaoTarjanMaximumFlow> maxflow;
   void run_impl() override;
    // void init_flow() override;
    // void init_circulation() override;
   //  std::int64_t cp(NetworKit::node, NetworKit::node);

   //  NetworKit::Graph generate_graph_for_sp(std::map<NetworKit::Edge, std::pair<int, NetworKit::node>>&);
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
   //  void contract_nodes(NetworKit::node, NetworKit::node);
   //  void delta_scaling_phase();
   //  void contraction_phase();
   //  void apply_reduced_costs();
   //  void uncontract_and_compute_flow();
   //  void fill_distances_uncapacitated(std::vector<double>&);
   //  void scaling_augmentation(NetworKit::node, NetworKit::node);
    NetworKit::Graph original_graph;
    std::vector<std::int64_t> potential;

    // potential that is computed for the original network
    std::vector<int64_t> potential_computed;


    std::pair<NetworKit::node, NetworKit::node> uncapacitated_nodes_bounds;
    bool is_added_uncapacitated(NetworKit::node v) const;

    void computeFinalFlows();

public:
    OrlinMCF(Koala::MCFlowNetwork& network) : MinimumCostFlow(network) {
      original_graph = network.getGraph();
    }

    int64_t getFlow(const NetworKit::Edge& edge) override; 

};

} /* namespace Koala */

 
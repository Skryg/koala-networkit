#pragma once

#include <set>
#include <unordered_set>
#include <list>
#include <cstdint>

#include <flow/MinimumCostFlow.hpp>
#include <flow/minimum_cost_flow/MCFlowNetwork.hpp>

namespace Koala {

class SuccessiveApproxMCC final : public MinimumCostFlow {
   struct Edge {
      uint32_t from, to;
      int64_t cost, capacity, flow;
   };
   std::vector<Edge> edges;
   std::vector<std::vector<uint64_t>> neigh_list;
   
   void run_impl() override;
   bool is_imbalanced();
   void initialize();
   void push(uint64_t eid);
   void relabel(NetworKit::node const&);
   void refine();
   void wave();
   bool discharge(NetworKit::node const&);
   
   double cp(uint64_t eid);
   int64_t uf(uint64_t eid);
   void force_flow(uint64_t eid, int64_t f);
   std::vector<double> potential;
   std::vector<int64_t> excess;
   uint32_t nodes_number{0};
   
   std::unordered_map<NetworKit::Edge, int64_t> computed_flow; 

   double epsi{0.};
 
   class DischargeList {
      public:
      virtual NetworKit::node getNext() { return 0; }
      virtual void moveToStart() {}
   };
      
   class ToposortList : public DischargeList {
    public:
      ToposortList(SuccessiveApproxMCC&);
      
      NetworKit::node getNext() override;
      void moveToStart() override;
    private:
      SuccessiveApproxMCC &approx;
      std::list<NetworKit::node> nodes;
      std::vector<bool> vis; 
      void dfs(NetworKit::node);
      
      std::list<NetworKit::node>::iterator it1;
      std::list<NetworKit::node>::iterator it2;
   };
 public:
   SuccessiveApproxMCC(const MCFlowNetwork& network) : MinimumCostFlow(network) {}
   int64_t getFlow(const NetworKit::Edge& edge);
};

} /* namespace Koala */

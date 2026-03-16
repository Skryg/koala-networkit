#pragma once

#include <flow/MinimumCostFlow.hpp>
#include <algorithm>

namespace Koala {

class EdmondsKarpMCF final : public MinimumCostFlow {
   void run_impl() override;
   void initialize();
   void deltaScalingPhase();
   void send(NetworKit::node s, NetworKit::node t, std::int64_t flow);
   NetworKit::Graph getDeltaResidual();
   std::int64_t cp(NetworKit::node, NetworKit::node);
   std::int64_t delta = 0;
   std::unordered_map<NetworKit::node, std::int64_t> potential;

 public: 
   EdmondsKarpMCF(MCFlowNetwork const& network) : MinimumCostFlow(network) {} 
};

} /* namespace Koala */
#pragma once

#include <flow/MinimumCostFlow.hpp>
#include <set>

namespace Koala{

class SuccessiveApproxMCF final : public MinimumCostFlow {
    using MinimumCostFlow::MinimumCostFlow;
    private:
    void runImpl();
    void initialize();
    void push_relabel(NetworKit::node const&);
    void push(NetworKit::node const&, NetworKit::node const&);
    void relabel(NetworKit::node const&);
    void refine();

    int cp(NetworKit::node const&, NetworKit::node const&);
    int uf(NetworKit::node const&, NetworKit::node const&);
    void force_flow(NetworKit::node const&, NetworKit::node const&, int);
    node_map<double> price;
    node_map<int> excess;
    node_map<int> pr_id;
    std::unordered_set<NetworKit::node> active; 

    double epsi{0.};
};

} /* namespace Koala */
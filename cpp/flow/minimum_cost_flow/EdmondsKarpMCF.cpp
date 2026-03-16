#include <flow/minimum_cost_flow/EdmondsKarpMCF.hpp>
#include <shortest_path/Dijkstra.hpp>
#include <vector>

using node = NetworKit::node;
using edgeid = NetworKit::edgeid;
using edgeweight = NetworKit::edgeweight;
using int64 = std::int64_t;

namespace Koala {

void EdmondsKarpMCF::initialize() {
    potential.clear();
    NetworKit::edgeweight maxWeight = 0;
    network.makeConnected();
    
    for (auto edge : network.getGraph().edgeWeightRange()) {
        maxWeight = std::max(maxWeight, edge.weight);
    }
    delta = 1;
    while (delta < lround(maxWeight)) delta <<= 1;
}

std::int64_t EdmondsKarpMCF::cp(node u, node v) {
    return network.getCost(u, v) - potential[u] + potential[v];
}

void EdmondsKarpMCF::send(node u, node v, int64 value) {
    network.pushFlow(u, v, value);
}

NetworKit::Graph EdmondsKarpMCF::getDeltaResidual() {
    NetworKit::Graph deltaResidual(network.getGraph().numberOfNodes(), true, true);
    network.getGraph().forEdges([&](node u, node v, edgeweight weight) {
        long long reducedCost = cp(u, v);
        int f = network.getFlow(u, v);

        if (f + delta <= lround(weight)) {
            deltaResidual.addEdge(u, v, reducedCost);
        }
        if (f >= delta) {
            deltaResidual.addEdge(v, u, -reducedCost);
        }
    });
    return deltaResidual;
}

void EdmondsKarpMCF::deltaScalingPhase() {
    NetworKit::Graph const& graph = network.getGraph();
    graph.forEdges([&](node u, node v, edgeweight weight) {
        long long f = network.getFlow(u, v);

        if(f >= delta && cp(u, v) > 0) {
            send(u, v, -f);
        }
        else if (f + delta <= lround(weight) && cp(u, v) < 0){ 
            send(u, v, lround(weight) - f);
        }
    });

    std::vector<node> S, T;
    graph.forNodes([&](node v){
        int ex = network.getExcess(v);
        if (ex >= delta) S.push_back(v);
        else if (ex <= -delta) T.push_back(v);
    });

    while (!S.empty() && !T.empty()) {
        auto deltaResidual = getDeltaResidual();
        node k = S.back();
        node l = T.back();

        // shortest path
        auto dijkstra = Dijkstra<FibonacciHeap>(deltaResidual, k, true);
        dijkstra.run();
        std::vector<node> path = dijkstra.getPath(l);
        std::vector<double> distances = dijkstra.getDistances();
        
        // update potential
        for(node v : graph.nodeRange()) {
            potential[v] -= lround(distances[v]);
        }

        // augment
        for (int i=0; i<path.size()-1; i++) {
            node p = path[i], q = path[i+1];

            if (network.getFlow(q, p) >= delta) {
                send(q, p, -delta);
            } else {
                send(p, q, delta);
            }
        }

        // update S, T
        if (network.getExcess(k) < delta) S.pop_back();
        if (network.getExcess(l) > -delta) T.pop_back();
    }
}

void EdmondsKarpMCF::run_impl() {
    initialize();

    while (delta >= 1) {
        deltaScalingPhase();
        delta /= 2;
    }

    min_cost = 0;
    network.getGraph().forEdges([&](node u, node v, edgeid eid) {
        min_cost += network.getFlow(u, v) * network.getCost(u, v);
    });
}

} /* namespace Koala */
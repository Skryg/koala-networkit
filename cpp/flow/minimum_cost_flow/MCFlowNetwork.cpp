#include <flow/minimum_cost_flow/MCFlowNetwork.hpp>
#include <limits>

using node = NetworKit::node;
using Edge = NetworKit::Edge;
using Graph = NetworKit:: Graph;
using int64 = std::int64_t;

namespace Koala {

    MCFlowNetwork::MCFlowNetwork(Graph const& g, bool circulation = false) : graph(g) {
        if (graph.isWeighted()) {
            graph.forEdges([&](node u, node v, NetworKit::edgeweight weight, NetworKit::edgeid _) {
                capacity[{u, v}] = static_cast<int64>(weight < 0 ? weight - 0.5 : weight + 0.5);
            });
        } else {
            graph.forEdges([&](node u, node v) {
                capacity[{u, v}] = std::numeric_limits<int64>::max();
            });
        }
    }

    MCFlowNetwork::MCFlowNetwork(Graph const& g, std::unordered_map<Edge, int64> const& cost, bool circulation = false) 
        : MCFlowNetwork(g, circulation) {
        this->cost = cost;
    }

    MCFlowNetwork::MCFlowNetwork(Graph const& g, std::unordered_map<Edge, int64> const& cost, std::unordered_map<node, int64> const& ex,
        bool circulation = false) : MCFlowNetwork(g, cost, circulation) {
        excess = ex;
    }


    Graph& MCFlowNetwork::getGraph() {
        return graph;
    };

    node MCFlowNetwork::addNode(int64 ex = 0) {
        node newNode = graph.addNode();
        excess[newNode] = ex;
        return newNode;
    }

    void MCFlowNetwork::addEdge(node s, node t, int64 cost = 0, int64 capacity = 0) {
        graph.addEdge(s, t, capacity);
        if (graph.isWeighted())
            this->capacity[{s,t}] = capacity;
        this->cost[{s,t}] = cost; 
    }
    
    void MCFlowNetwork::makeConnected() {
        int64 maxCost{0}; 
        for (auto [edge, cost] : cost) {
            maxCost = std::max(maxCost, (int64)std::abs(cost));
        }
        
        maxCost *= graph.numberOfEdges() + 1;
        
        int64 sumB{0};
        for (auto [nd, bval] : excess) {
            if (bval > 0) sumB += bval;
        }

        NetworKit::node sx = graph.addNode();
        NetworKit::node sx2 = graph.addNode();
        graph.addEdge(sx, sx2, std::numeric_limits<NetworKit::edgeweight>::infinity());
        capacity[{sx, sx2}] = std::numeric_limits<int64>::infinity();
        cost[{sx, sx2}] = maxCost;
        graph.forNodes([&](NetworKit::node u) {
            if (sx == u || sx2 == u) return;
            auto bound = graph.upperEdgeIdBound();
            this->capacity[{u, sx}] = this->capacity[{sx2, u}] = std::numeric_limits<int64>::infinity();
            graph.addEdge(u, sx, std::numeric_limits<NetworKit::edgeweight>::infinity());
            graph.addEdge(sx2, u, std::numeric_limits<NetworKit::edgeweight>::infinity());
        });
    }

    void MCFlowNetwork::makeUncapacitated() {
        NetworKit::Graph g(graph.upperNodeIdBound());

        graph.forEdges([&](node u, node v, NetworKit::edgeweight weight) {
            node w = g.addNode();
            int64 cap = capacity[{u, v}];
            if (cap == std::numeric_limits<int64>::infinity())
                return;
            excess[v] += cap;
            excess[w] -= cap;
            capacity.erase({u, v});
            capacity[{u, w}] = capacity[{v, w}] = std::numeric_limits<int64>::infinity();
            cost[{u, w}] = cost[{u, v}];
            cost.erase({u, v});
            g.addEdge(u, w, std::numeric_limits<NetworKit::edgeweight>::infinity());
            g.addEdge(v, w, std::numeric_limits<NetworKit::edgeweight>::infinity());
        });

        graph = g;
    }

    void MCFlowNetwork::makeCostsNonNegative() {
        std::vector<NetworKit::Edge> negativeEdges;
        graph.forEdges([&](node u, node v) {
            if (cost[{u, v}] < 0) { 
                negativeEdges.push_back({u, v});
            }
        });

        for (const NetworKit::Edge& edge : negativeEdges) {
            int64 cap = capacity[edge];
            auto [u, v] = edge;
        
            excess[v] += cap;
            excess[u] -= cap;
            graph.removeEdge(edge.u, edge.v);
            graph.addEdge(edge.v, edge.u, static_cast<double>(cap));
            cost[{v, u}] = -cost[{u,v}];
            cost[{u, v}] = 0;
        }
    }

} /* namespace Koala */


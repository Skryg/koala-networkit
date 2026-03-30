#include <flow/minimum_cost_flow/MCFlowNetwork.hpp>
#include <limits>

namespace Koala {
    using node = NetworKit::node;
    using Edge = NetworKit::Edge;
    using Graph = NetworKit:: Graph;
    using int64 = std::int64_t;

    MCFlowNetwork::MCFlowNetwork(Graph const& g) : graph(g) {
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

    MCFlowNetwork::MCFlowNetwork(Graph const& g, std::unordered_map<node, int64> const& ex) 
        : MCFlowNetwork(g) {
        excess = ex;
        primaryExcess = ex;
    }
    MCFlowNetwork::MCFlowNetwork(Graph const& g, std::unordered_map<node, int64> const& ex,
        std::unordered_map<Edge, int64> cost) : MCFlowNetwork(g, ex) {
        this->cost = cost;
    }

    Graph const& MCFlowNetwork::getGraph() const {
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

    void MCFlowNetwork::pushFlow(node s, node t, int64 flow) {
        int64 maxAdd = capacity[{s,t}] - this->flow[{s,t}];
        flow = !uncapacitated ? std::min(maxAdd, flow) : flow;
        excess[s] -= flow;
        excess[t] += flow;
        this->flow[{s,t}] += flow;
        this->flow[{t,s}] -= flow;
    }

    void MCFlowNetwork::setFlow(node s, node t, int64 value) {
        int64 diff = value - flow[{s,t}];
        flow[{s,t}] += diff;
        flow[{t,s}] -= diff;
        excess[s] -= diff;
        excess[t] += diff;
    }

    int64 MCFlowNetwork::getFlow(node s, node t) const {
        auto it = flow.find({s,t});
        if (it != flow.end())
            return it->second;
        return 0;
    }

    int MCFlowNetwork::getCost(node s, node t) const {
        auto it = cost.find({s,t});
        if (it != cost.end())
            return it->second;
        return 0;
    }  

    void MCFlowNetwork::setCost(node s, node t, int64 cost) {
        this->cost[{s,t}] = cost;
    }

    int64 MCFlowNetwork::getCapacity(node s, node t) const {
        auto it = capacity.find({s,t});
        if (it != capacity.end())
            return it->second;
        return 0;
    }

    void MCFlowNetwork::setCapacity(node s, node t, int64 value) {
        capacity[{s,t}] = value;
    }

    int64 MCFlowNetwork::getExcess(node u) const {
        auto it = excess.find(u);
        if (it != excess.end())
            return it->second;
        return 0;
    }

    void MCFlowNetwork::setExcess(node u, int64 ex) {
        int64 diff = ex - primaryExcess[u];
        primaryExcess[u] = ex;
        excess[u] += diff;
    }

    void MCFlowNetwork::forResidualEdgesOf(node u, std::function<void(NetworKit::node, NetworKit::node)> const& func) const {
        graph.forNeighborsOf(u, [&](node v) {
            int64 residualCapacity = 1;
            
            if (!uncapacitated) {
                auto capacityIt = capacity.find({u,v});
                int64 vCapacity = capacityIt != capacity.end() ? capacityIt->second : 0;
                auto flowIt = flow.find({u,v});
                int64 vFlow = flowIt != flow.end() ? flowIt->second : 0;
                residualCapacity = vCapacity - vFlow;
            }

            if (residualCapacity > 0) {
                func(u, v);
            }
        });

        graph.forInNeighborsOf(u, [&](node v) {
            auto flowIt = flow.find({u,v});
            int64 vFlow = flowIt != flow.end() ? flowIt->second : 0;

            int64 residualCapacity = vFlow;
            if (residualCapacity > 0) {
                func(u, v);
            }
        });
    }

    void MCFlowNetwork::makeConnected() {
        int64 maxCost{0}; 
        for (auto [edge, cost] : cost) {
            maxCost = std::max(maxCost, (int64)std::abs(cost));
        }
        
        maxCost *= graph.numberOfEdges() + 1;
        
        int64 sumB{0};
        for (auto [nd, bval] : primaryExcess) {
            if (bval > 0) sumB += bval;
        }

        NetworKit::node sx = graph.addNode();
        graph.forNodes([&](NetworKit::node u) {
            if (sx == u) return;
            auto bound = graph.upperEdgeIdBound();
            this->cost[{u, sx}] = this->cost[{sx, u}] = maxCost;
            graph.addEdge(u, sx, sumB);
            graph.addEdge(sx, u, sumB);
        });
    }

    void MCFlowNetwork::makeUncapacitated() {
        // TODO
    }

    int64 MCFlowNetwork::getFlowCost() const {
        int64 sum{0};
        graph.forEdges([&](NetworKit::node u, NetworKit::node v) {
            try {
                int64 edgeCost = flow.at({u, v}) * cost.at({u, v});
                sum += edgeCost;

            } catch (const std::out_of_range& e) {}
        });
        
        return sum;
    }

    bool MCFlowNetwork::isLegalFlow() const {
        for (auto [node, value] : excess) {
            if (value != 0) return false;
        }

        if (!uncapacitated) {
            bool capacitySatisfied = true;
            graph.forEdges([&](NetworKit::node u, NetworKit::node v) {
                try {
                    if (flow.at({u, v}) > capacity.at({u, v})) 
                        capacitySatisfied = false;
                } catch (std::out_of_range const& ex) {}
            });
            if (!capacitySatisfied) {
                return false;
            }
        }
        return true;
    }

} /* namespace Koala */


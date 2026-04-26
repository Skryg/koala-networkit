#include <flow/minimum_cost_flow/EdmondsKarpMCF.hpp>
#include <shortest_path/Dijkstra.hpp>
#include <vector>
#include <climits>
#include <iostream>

// #define DEBUG_DUMP

#ifdef DEBUG_DUMP
#define DBG(x) std::cerr << x
#else 
#define DBG(x)
#endif

using node = NetworKit::node;
using edgeid = NetworKit::edgeid;
using edgeweight = NetworKit::edgeweight;

namespace Koala {

void EdmondsKarpMCF::initialize() {
    auto& graph = network.getGraph();
    network.makeConnected();
    n = graph.upperNodeIdBound();

    excess.assign(n, 0);
    for (auto [key, value] : network.excess) {
        excess[key] = value;
    }
    potential.assign(n, 0);
    edges.assign(2 * graph.numberOfEdges(), Edge());
    int ptr = 0;
    
    neigh_list.assign(n, std::vector<uint32_t>());
    graph.forNodes([&](node u) { 
        graph.forNeighborsOf(u, [&](node v) {
            uint32_t from = static_cast<uint32_t>(u);
            uint32_t to = static_cast<uint32_t>(v);
            int64_t cost = network.cost[{u, v}];
            int64_t capacity = network.capacity[{u, v}];
            neigh_list[from].push_back(2*ptr);
            edges[2*ptr] = {
                from, to,
                cost, capacity, 0LL
            };

            neigh_list[to].push_back(2*ptr+1);
            edges[2*ptr + 1] = {
                to, from, 
                -cost, 0LL, 0LL
            };
            ++ptr;
        });
    });


    for (int i = 0; i < edges.size(); ++i) {
        DBG(i << " -> from: " << edges[i].from 
            << " to: " << edges[i].to << " capacity: " << edges[i].capacity
            << " cost: " << edges[i].cost << '\n');
    }
}

std::vector<std::pair<int64_t, uint64_t>> EdmondsKarpMCF::dijkstra(uint32_t source, int64_t delta) {
    std::set<std::pair<int64_t, uint32_t>> pq;
    std::vector<std::pair<int64_t, uint64_t>> dist(n, {std::numeric_limits<int64_t>::max(), 0});
    dist[source] = {0, 0};
    pq.insert({0, source});
    std::vector<bool> visited(n, false);
    DBG("Dijkstra\n");
    while (!pq.empty()) {
        auto [d, u] = *pq.begin();
        pq.erase(pq.begin());

        if (visited[u]) continue;
        visited[u] = true;
        
        for (uint32_t edge_idx : neigh_list[u]) {
            const Edge& edge = edges[edge_idx];
            if (edge.capacity >= edge.flow + delta) {
                assert(edge.from == u);
                int64_t new_dist = d + edge.cost - potential[u] + potential[edge.to];
                if (new_dist < dist[edge.to].first) {
                    pq.erase({dist[edge.to].first, edge.to});
                    dist[edge.to] = {new_dist, edge_idx};
                    pq.insert({new_dist, edge.to});
                }
            }
        }
    }

    return dist;
}

void EdmondsKarpMCF::send(uint64_t edgeid, int64_t value) {
    edges[edgeid].flow += value;
    edges[edgeid^1].flow -= value;
}

void EdmondsKarpMCF::augmenting_phase(uint32_t s, uint32_t t, int64_t delta) {
    auto dist = dijkstra(s, delta);
    DBG("AUGMENTING delta: " << delta << " From: " << s << " To: " << t <<  "\n");
    uint32_t ptr = t;
    while (s != ptr) {
        auto [_, edgeid] = dist[ptr];
        DBG("From: "<<edges[edgeid].from << " To: " << edges[edgeid].to << " Residual cap: " <<  edges[edgeid].capacity-edges[edgeid].flow << '\n');
        send(edgeid, delta);
        ptr = edges[edgeid].from;

    }
    excess[t] += delta;
    excess[s] -= delta;

    for (uint32_t i = 0; i < n; i++) {
        if (dist[i].first != LLONG_MAX) {
            potential[i] -= dist[i].first;
        }
    }
}

void EdmondsKarpMCF::delta_scaling_phase(int64_t delta) {
    NetworKit::Graph const& graph = network.getGraph();
    for (int i = 0; i < edges.size(); ++i) {
        Edge& edge = edges[i];
        if (edge.capacity - edge.flow >= delta
            && edge.cost - potential[edge.from] + potential[edge.to] <= 0) {
                send(i, delta);
                excess[edge.from] -= delta;
                excess[edge.to] += delta;
        }
    }

    std::stack<node> S, T;
    graph.forNodes([&](node v){
        int ex = excess[v];
        if (ex >= delta) S.push(v);
        else if (ex <= -delta) T.push(v);
    });

    while (!S.empty() && !T.empty()) {
        node k = S.top();
        node l = T.top();

        augmenting_phase(k, l, delta);
        // update S, T
        if (excess[k] < delta) S.pop();
        if (excess[l] > -delta) T.pop();
    }
}

void EdmondsKarpMCF::run_impl() {
    initialize();
    
    int64_t delta{1}; 
    int i=0;
    DBG("excess\n");
    for (auto e : excess) {
        DBG(i++ <<": " << e <<'\n');
        while (delta < e) {
            delta <<= 1;
        }
    }

    while (delta >= 1) {
        delta_scaling_phase(delta);
        delta /= 2;
    }

    min_cost = 0;
    for (const Edge& edge : edges) {
        computed_flow[{edge.from, edge.to}] = edge.flow;
        min_cost += edge.flow * edge.cost;
    } 
    min_cost /= 2;
}

int64_t EdmondsKarpMCF::getFlow(NetworKit::Edge const& edge) {
    return computed_flow[edge];
}

} /* namespace Koala */
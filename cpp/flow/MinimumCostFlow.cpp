#include <flow/MinimumCostFlow.hpp>
#include <networkit/graph/Graph.hpp>

namespace Koala {

using index = NetworKit::index;
using int64 = std::int64_t;


// bool MinimumCostFlow::isOk() const {
//     return feasible;
// }

// void MinimumCostFlow::construct_circulation_from_flow() {
//     NetworKit::node n = graph.addNode();
//     graph.forNodes(
//         [&](NetworKit::node v) {
//             if (b[v] > 0) {
//                 graph.addEdge(n, v);
//                 upperbound[{n, v}] = excess[v];
//                 lowerbound[{n, v}] = excess[v];
//             }
//             if (b[v] < 0) {
//                 graph.addEdge(v, n);
//                 upperbound[{v, n}] = excess[v];
//                 lowerbound[{v, n}] = excess[v];
//             }
//         });
// }

// void MinimumCostFlow::construct_flow_from_circulation() {
//     NetworKit::Graph G(graph.numberOfNodes(), true, true, true);

//     graph.forEdges([&](NetworKit::node u, NetworKit::node v) {
//         b[u] -= lowerbound[{u, v}];
//         b[v] += lowerbound[{u, v}];
//         G.addEdge(u, v, upperbound[{u, v}] - lowerbound[{u, v}]);
//     });

//     graph = G;

// }

// void MinimumCostFlow::make_uncapacitated() {
//     if (!graph.isWeighted()) return;
//     int nodes = graph.numberOfNodes();
//     NetworKit::Graph newGraph(nodes, false, true, true);
//     node_map<long long> newB;
//     edge_map<long long> newCosts;

//     NetworKit::node firstAdded = graph.upperNodeIdBound();
//     uncapacitated_nodes_bounds = {firstAdded, firstAdded-1};
//     graph.forEdges(
//     [&](NetworKit::node u, NetworKit::node v, NetworKit::edgeweight weight) {
//         auto k = newGraph.addNode();
//         uncapacitated_nodes_bounds.second = k;

//         newGraph.addEdge(u,k);
//         newGraph.addEdge(v,k);
//         newB[u] = b[u];
//         newB[v] = weight + b[v];
//         newB[k] = -weight;

//         newCosts[{u, k}] = 0;
//         newCosts[{v, k}] = costs[{u, v}];
//         uncapacitated_mapping[{u, v}] = {v, k};
//         // flowEdgeToId[{u,v}] = {u, v};
//     });

//     b = newB;
//     graph = newGraph;
//     costs = newCosts;
// }

} /* namespace Koala */

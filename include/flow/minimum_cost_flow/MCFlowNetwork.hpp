#include <networkit/graph/Graph.hpp>
#include <networkit/graph/EdgeIterators.hpp>
#include <unordered_map>
#include <vector>
namespace Koala {

class BaseMCFlowNetwork {
 public:
    virtual NetworKit::Graph const& getGraph() const = 0;
    virtual NetworKit::node addNode(std::int64_t ex) = 0;
    virtual void addEdge(NetworKit::node s, NetworKit::node t, std::int64_t cost = 0, std::int64_t capacity = 0) = 0;
    
    virtual void pushFlow(NetworKit::node s, NetworKit::node t, std::int64_t value) = 0;
    virtual void setFlow(NetworKit::node s, NetworKit::node t, std::int64_t value) = 0;
    virtual std::int64_t getFlow(NetworKit::node s, NetworKit::node t) const = 0;
    
    virtual int getCost(NetworKit::node s, NetworKit::node t) const = 0;
    virtual void setCost(NetworKit::node s, NetworKit::node t, std::int64_t cost) = 0;

    virtual std::int64_t getCapacity(NetworKit::node s, NetworKit::node t) const = 0;
    virtual void setCapacity(NetworKit::node s, NetworKit::node t, std::int64_t value) = 0;
    
    virtual std::int64_t getExcess(NetworKit::node u) const = 0;
    virtual void setExcess(NetworKit::node u, std::int64_t ex) = 0;

    virtual void forResidualEdgesOf(NetworKit::node u, std::function<void(NetworKit::node, NetworKit::node)> const& func) const;
    virtual void makeConnected() = 0;
    virtual void makeUncapacitated() = 0;
};

class MCFlowNetwork : public BaseMCFlowNetwork {
    std::unordered_map<NetworKit::Edge, std::int64_t> flow;
    std::unordered_map<NetworKit::Edge, std::int64_t> cost;
    std::unordered_map<NetworKit::Edge, std::int64_t> capacity;

    std::unordered_map<NetworKit::node, std::int64_t> primaryExcess;
    std::unordered_map<NetworKit::node, std::int64_t> excess;
    NetworKit::Graph graph;
    bool uncapacitated = false;
 public:
    
    MCFlowNetwork(NetworKit::Graph const& g);
    MCFlowNetwork(NetworKit::Graph const& g, std::unordered_map<NetworKit::node, std::int64_t> const& ex);
    MCFlowNetwork(NetworKit::Graph const& g, std::unordered_map<NetworKit::node, std::int64_t> const& ex,
        std::unordered_map<NetworKit::Edge, std::int64_t> cost);

    virtual NetworKit::Graph const& getGraph() const override;

    virtual NetworKit::node addNode(std::int64_t ex = 0) override;
    virtual void addEdge(NetworKit::node s, NetworKit::node t, std::int64_t cost = 0, std::int64_t capacity = 0) override;
    
    virtual void pushFlow(NetworKit::node s, NetworKit::node t, std::int64_t value) override;
    virtual void setFlow(NetworKit::node s, NetworKit::node t, std::int64_t value) override;
    virtual std::int64_t getFlow(NetworKit::node s, NetworKit::node t) const override;
    
    virtual int getCost(NetworKit::node s, NetworKit::node t) const override;
    virtual void setCost(NetworKit::node s, NetworKit::node t, std::int64_t cost) override;

    virtual std::int64_t getCapacity(NetworKit::node s, NetworKit::node t) const override;
    virtual void setCapacity(NetworKit::node s, NetworKit::node t, std::int64_t value) override;
    
    virtual std::int64_t getExcess(NetworKit::node u) const override;
    virtual void setExcess(NetworKit::node u, std::int64_t ex) override;

    virtual void forResidualEdgesOf(NetworKit::node u, std::function<void(NetworKit::node, NetworKit::node)> const& func) const override;
    virtual void makeConnected() override;
    virtual void makeUncapacitated() override;
};

}
#include "valuetrace/Slice.h"

#include <deque>
#include <limits>
#include <stdexcept>

namespace valuetrace {
namespace {

DirectionSlice walk(const DependencyGraph& graph,
                    const std::vector<NodeId>& seeds,
                    Direction direction, unsigned limit) {
    // A shorter path leaves more budget. Revisit a node only when its minimum
    // depth decreases; local cycles therefore cannot make traversal infinite.
    const auto unseen = std::numeric_limits<unsigned>::max();
    std::vector<unsigned> depths(graph.size(), unseen);
    std::deque<NodeId> pending;
    for (auto seed : seeds) {
        graph.node(seed);
        if (depths[seed] != 0) {
            depths[seed] = 0;
            pending.push_back(seed);
        }
    }
    const auto neighbors = [&](NodeId id) -> const std::vector<std::size_t>& {
        return direction == Direction::Upstream ? graph.incoming(id) : graph.outgoing(id);
    };
    const auto nextNode = [&](const Edge& edge) {
        return direction == Direction::Upstream ? edge.from : edge.to;
    };
    while (!pending.empty()) {
        const auto current = pending.front();
        pending.pop_front();
        for (auto edgeId : neighbors(current)) {
            const auto& edge = graph.edge(edgeId);
            const bool crossesCall = edge.kind == EdgeKind::CallBoundary;
            if (crossesCall && depths[current] >= limit) {
                continue;
            }
            const auto next = nextNode(edge);
            const auto depth = depths[current] + (crossesCall ? 1U : 0U);
            if (depth < depths[next]) {
                depths[next] = depth;
                pending.push_back(next);
            }
        }
    }

    DirectionSlice result;
    for (NodeId id = 0; id < graph.size(); ++id) {
        if (depths[id] == unseen) {
            continue;
        }
        result.nodes.push_back({id, depths[id]});
        for (auto edgeId : neighbors(id)) {
            const auto& edge = graph.edge(edgeId);
            if (edge.kind == EdgeKind::CallBoundary && depths[id] >= limit) {
                result.boundaries.push_back({id, nextNode(edge), edgeId});
            }
        }
    }
    return result;
}

} // namespace

SliceResult buildSlice(const DependencyGraph& graph, const SliceQuery& query) {
    if (query.parameters.empty()) {
        throw std::invalid_argument("select at least one parameter");
    }
    return {walk(graph, query.parameters, Direction::Upstream, query.upDepth),
            walk(graph, query.parameters, Direction::Downstream, query.downDepth)};
}

} // namespace valuetrace

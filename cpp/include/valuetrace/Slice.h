#pragma once

#include "valuetrace/DependencyGraph.h"

#include <vector>

namespace valuetrace {

enum class Direction { Upstream, Downstream };

struct SliceQuery {
    std::vector<NodeId> parameters;
    unsigned upDepth = 0;
    unsigned downDepth = 0;
};

struct ReachedNode {
    NodeId node;
    unsigned callDepth;
};

struct DepthBoundary {
    NodeId reached;
    NodeId notExpanded;
    std::size_t edge;
};

struct DirectionSlice {
    std::vector<ReachedNode> nodes;
    std::vector<DepthBoundary> boundaries;
};

struct SliceResult {
    DirectionSlice upstream;
    DirectionSlice downstream;
};

// Only builds a bounded dependency slice. A slice is not a safety proof.
SliceResult buildSlice(const DependencyGraph& graph, const SliceQuery& query);

} // namespace valuetrace

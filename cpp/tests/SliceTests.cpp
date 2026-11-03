#include "valuetrace/Slice.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace valuetrace;

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

bool contains(const DirectionSlice& slice, NodeId node, unsigned depth) {
    return std::any_of(slice.nodes.begin(), slice.nodes.end(), [&](const auto& reached) {
        return reached.node == node && reached.callDepth == depth;
    });
}

int main() {
    try {
        DependencyGraph graph;
        const auto a = graph.addNode({"A", "x", 32, NodeKind::Parameter, "root"});
        const auto b = graph.addNode({"B", "x", 32, NodeKind::Parameter, "call1"});
        const auto local = graph.addNode({"B", "x * 4", 32, NodeKind::Operation, "call1"});
        const auto c = graph.addNode({"C", "x", 32, NodeKind::Parameter, "call1/call2"});
        graph.addEdge(a, b, EdgeKind::CallBoundary, "call1");
        graph.addEdge(b, local, EdgeKind::Local);
        graph.addEdge(local, c, EdgeKind::CallBoundary, "call2");

        const auto zero = buildSlice(graph, {{b}, 0, 0});
        require(contains(zero.downstream, local, 0), "local dependency must not consume depth");
        require(!contains(zero.upstream, a, 1), "up depth zero must stop at caller");
        require(zero.upstream.boundaries.size() == 1, "caller boundary must be reported");
        require(zero.downstream.boundaries.size() == 1, "callee boundary must be reported");

        const auto asymmetric = buildSlice(graph, {{b}, 1, 0});
        require(contains(asymmetric.upstream, a, 1), "up depth one must include caller");
        require(!contains(asymmetric.downstream, c, 1), "down depth must be independent");
        const auto full = buildSlice(graph, {{b}, 1, 1});
        require(contains(full.downstream, c, 1), "down depth one must include callee");

        const auto group = buildSlice(graph, {{a, c, a}, 0, 0});
        require(group.upstream.nodes.size() == 2, "parameter groups must deduplicate seeds");

        graph.addEdge(local, b, EdgeKind::Local);
        const auto cycle = buildSlice(graph, {{b}, 0, 0});
        require(cycle.downstream.nodes.size() == 2, "local cycle must terminate");

        bool rejected = false;
        try { buildSlice(graph, {{}, 1, 1}); }
        catch (const std::invalid_argument&) { rejected = true; }
        require(rejected, "empty parameter group must be rejected");

        const auto otherCall = graph.addNode({"B", "x", 32, NodeKind::Parameter, "other-call"});
        rejected = false;
        try { graph.addEdge(b, otherCall, EdgeKind::Local); }
        catch (const std::invalid_argument&) { rejected = true; }
        require(rejected, "local edges must not mix call contexts");

        std::cout << "Bounded slicing tests passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

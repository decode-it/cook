#ifndef HEADER_cook_algo_TopologicalOrder_hpp_ALREADY_INCLUDED
#define HEADER_cook_algo_TopologicalOrder_hpp_ALREADY_INCLUDED

#include "cook/algo/DependencyGraph.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/Result.hpp"
#include "gubg/graph/TopologicalSort.hpp"

namespace cook { namespace algo {

// topological dependencies have an element with zero dependencies at index 0
template <typename DependencyGraph, typename OutIterator>
Result make_TopologicalOrder(const DependencyGraph & dependency_graph, OutIterator out_iterator)
{
    MSS_BEGIN(Result);
    using Vertex = typename gubg::graph::Traits<DependencyGraph>::vertex_descriptor;

    std::vector<Vertex> vertices(gubg::graph::num_vertices(dependency_graph));

    MSG_MSS(gubg::graph::construct_topological_order(dependency_graph, vertices.rbegin()), Error, "Cyclic dependency detected");
    std::transform(RANGE(vertices), out_iterator, [&](Vertex v) { return gubg::graph::vertex_label(v, dependency_graph); });

    MSS_END();
}


} }

#endif

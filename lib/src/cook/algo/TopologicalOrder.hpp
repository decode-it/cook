#ifndef HEADER_cook_algo_TopologicalOrder_hpp_ALREADY_INCLUDED
#define HEADER_cook_algo_TopologicalOrder_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"
#include "cook/Result.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/topological_sort.hpp"

namespace cook { namespace algo {


// topological dependencies have an element with zero dependencies at index 0
template <typename DependencyGraph, typename OutIterator>
Result make_TopologicalOrder(const DependencyGraph & dependency_graph, OutIterator out_iterator)
{
    MSS_BEGIN(Result);
    using Vertex = typename boost::graph_traits<DependencyGraph>::vertex_descriptor;

    std::vector<Vertex> vertices(boost::num_vertices(dependency_graph));

    try
    {
        boost::topological_sort(dependency_graph, vertices.begin());
    }
    catch(boost::not_a_dag)
    {
        MSG_MSS(false, Error, "Cyclic dependency detected");
    }

    std::transform(RANGE(vertices), out_iterator, [&](Vertex v) { return dependency_graph[v]; });

    MSS_END();
}

Result make_TopologicalOrder(const std::list<model::Recipe *> & roots, std::list<model::Recipe*> & make_TopologicalOrder);
Result make_TopologicalOrder(model::Recipe * root, std::list<model::Recipe*> & make_TopologicalOrder);


} }

#endif

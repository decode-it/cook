#ifndef HEADER_cook_algo_ComponentGraph_hpp_ALREADY_INCLUDED
#define HEADER_cook_algo_ComponentGraph_hpp_ALREADY_INCLUDED

#include "cook/algo/DependencyGraph.hpp"
#include "gubg/graph/AdjacencyList.hpp"

namespace cook { namespace algo {

namespace  detail {

template <typename Map, typename DependencyGraph, typename ComponentGraph>
void grow_(typename gubg::graph::Traits<DependencyGraph>::vertex_descriptor dep_seed, const DependencyGraph & dep_g, typename gubg::graph::Traits<ComponentGraph>::vertex_descriptor c, ComponentGraph & comp_g, Map & dep_comp_map)
{
    using DepV = typename gubg::graph::Traits<DependencyGraph>::vertex_descriptor;

    std::stack<DepV> todo;
    todo.push(dep_seed);

    auto & set = gubg::graph::vertex_label(c, comp_g);

    while(!todo.empty())
    {
        DepV v = todo.top();
        todo.pop();

        model::Recipe * cur_recipe = gubg::graph::vertex_label(v, dep_g);

        // already handled this verte
        if (!set.insert(cur_recipe).second)
            continue;

        // add the translation from the recipe -> its component
        dep_comp_map[cur_recipe] = c;

        // add all the dependencies to the same component if globbing is allowd
        if (cur_recipe->allows_early_globbing())
            for(const auto & e : gubg::graph::out_edges(v, dep_g))
                todo.push(gubg::graph::target(e, dep_g));

        for (const auto & e : gubg::graph::in_edges(v, dep_g))
        {
            auto u = gubg::graph::source(e, dep_g);
            model::Recipe * parent = gubg::graph::vertex_label(u, dep_g);

            if (parent->allows_early_globbing())
                todo.push(u);
        }

    }
}

}

template <typename DependencyGraph, typename ComponentGraph, typename Map>
Result make_ComponentGraph(const DependencyGraph & dependency_graph, ComponentGraph & component_graph, Map & dependency_vertex_to_component_vertex_map)
{
    MSS_BEGIN(Result);

    // construct the vertices of the component graph
    for(auto v : gubg::graph::vertices(dependency_graph))
    {
        model::Recipe * recipe = gubg::graph::vertex_label(v, dependency_graph);

        if (dependency_vertex_to_component_vertex_map.find(recipe) != dependency_vertex_to_component_vertex_map.end())
            continue;

        auto comp_v = gubg::graph::add_vertex(component_graph);
        detail::grow_(v, dependency_graph, comp_v, component_graph, dependency_vertex_to_component_vertex_map);
    }

    // construct the edges of the component graph
    for(const auto & e : gubg::graph::edges(dependency_graph))
    {
        model::Recipe * src_recipe = gubg::graph::vertex_label(gubg::graph::source(e, dependency_graph), dependency_graph);
        model::Recipe * dst_recipe = gubg::graph::vertex_label(gubg::graph::target(e, dependency_graph), dependency_graph);

        auto src_it = dependency_vertex_to_component_vertex_map.find(src_recipe);
        auto dst_it = dependency_vertex_to_component_vertex_map.find(dst_recipe);
        MSS(src_it != dependency_vertex_to_component_vertex_map.end());
        MSS(dst_it != dependency_vertex_to_component_vertex_map.end());

        auto src_v = src_it->second;
        auto dst_v = dst_it->second;
        if (src_v != dst_v)
            gubg::graph::add_edge(src_v, dst_v, component_graph);
    }

    MSS_END();
}

template <typename DependencyGraph, typename ComponentGraph>
Result make_ComponentGraph(const DependencyGraph & dependency_graph, ComponentGraph & component_graph)
{
    MSS_BEGIN(Result);

    std::unordered_map<model::Recipe * , typename gubg::graph::Traits<ComponentGraph>::vertex_descriptor> translation_map;
    MSS(make_ComponentGraph(dependency_graph, component_graph, translation_map));

    MSS_END();
}


} }

#endif

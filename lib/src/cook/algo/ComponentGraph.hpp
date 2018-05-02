#ifndef HEADER_cook_algo_ComponentGraph_hpp_ALREADY_INCLUDED
#define HEADER_cook_algo_ComponentGraph_hpp_ALREADY_INCLUDED

#include "cook/algo/DependencyGraph.hpp"
#include "boost/graph/adjacency_list.hpp"

namespace cook { namespace algo {

namespace  detail {

template <typename Map, typename DependencyGraph, typename ComponentGraph>
void grow_(typename boost::graph_traits<DependencyGraph>::vertex_descriptor dep_seed, const DependencyGraph & dep_g, typename boost::graph_traits<ComponentGraph>::vertex_descriptor c, ComponentGraph & comp_g, Map & dep_comp_map)
{
    using DepV = typename boost::graph_traits<DependencyGraph>::vertex_descriptor;

    std::stack<DepV> todo;
    todo.push(dep_seed);

    auto & set = comp_g[c];

    while(!todo.empty())
    {
        DepV v = todo.top();
        todo.pop();

        model::Recipe * cur_recipe = dep_g[v];

        // already handled this verte
        if (!set.insert(cur_recipe).second)
            continue;

        // add the translation from the recipe -> its component
        dep_comp_map[cur_recipe] = c;

        // add all the dependencies to the same component if globbing is allowd
        if (cur_recipe->allows_early_globbing())
            for(const auto & e : gubg::make_range(boost::out_edges(v, dep_g)))
                todo.push(boost::target(e, dep_g));

        for (const auto & e : gubg::make_range(boost::in_edges(v, dep_g)))
        {
            auto u = boost::source(e, dep_g);
            model::Recipe * parent = dep_g[u];

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
    for(auto p = boost::vertices(dependency_graph); p.first != p.second; ++p.first)
    {
        model::Recipe * recipe = dependency_graph[*p.first];

        if (dependency_vertex_to_component_vertex_map.find(recipe) != dependency_vertex_to_component_vertex_map.end())
            continue;

        auto comp_v = boost::add_vertex(component_graph);
        detail::grow_(*p.first, dependency_graph, comp_v, component_graph, dependency_vertex_to_component_vertex_map);
    }

    // construct the edges of the component graph
    for(const auto & e : gubg::make_range(boost::edges(dependency_graph)))
    {
        model::Recipe * src_recipe = dependency_graph[boost::source(e, dependency_graph)];
        model::Recipe * dst_recipe = dependency_graph[boost::target(e, dependency_graph)];

        auto src_it = dependency_vertex_to_component_vertex_map.find(src_recipe);
        auto dst_it = dependency_vertex_to_component_vertex_map.find(dst_recipe);
        MSS(src_it != dependency_vertex_to_component_vertex_map.end());
        MSS(dst_it != dependency_vertex_to_component_vertex_map.end());

        auto src_v = src_it->second;
        auto dst_v = dst_it->second;
        if (src_v != dst_v)
            boost::add_edge(src_v, dst_v, component_graph);
    }

    MSS_END();
}

template <typename DependencyGraph, typename ComponentGraph>
Result make_ComponentGraph(const DependencyGraph & dependency_graph, ComponentGraph & component_graph)
{
    MSS_BEGIN(Result);

    std::unordered_map<model::Recipe * , typename boost::graph_traits<ComponentGraph>::vertex_descriptor> translation_map;
    MSS(make_ComponentGraph(dependency_graph, component_graph, translation_map));

    MSS_END();
}


} }

#endif

#include "cook/process/Menu.hpp"
#include "cook/algo/Recipe.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/topological_sort.hpp"
#include <cassert>

namespace cook { namespace process {

namespace {

using DepG = boost::adjacency_list<boost::listS, boost::listS, boost::bidirectionalS, model::Recipe *>;
using CompG = boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS, std::set<model::Recipe *>>;
using DepV = boost::graph_traits<DepG>::vertex_descriptor;
using CompV = boost::graph_traits<CompG>::vertex_descriptor;

template <typename Map>
void grow_(DepV dep_seed, const DepG & dep_g, CompV c, CompG & comp_g, Map & dep_comp_map)
{
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

        // add the translation from v  -> its component
        dep_comp_map[v] = c;

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

template <typename It, typename Map>
It find_first_unseen_(It first, It last, const Map & map)
{
    while(first != last && map.find(*first) != map.end())
        ++first;

    return first;
}

template <typename It>
Result construct_component_graph_(const gubg::Range<It> & root_recipes, CompG & comp_g)
{
    MSS_BEGIN(Result);

    // construct a dependency graph
    DepG dep_g;
    std::unordered_map<model::Recipe *, DepV> translation_map;
    MSS(algo::construct_dependency_graph(root_recipes, dep_g, translation_map));

    std::unordered_map<DepV, CompV> dep_comp_map;

    // construct the vertices of the component graph
    auto p = boost::vertices(dep_g);
    while(p.first != p.second)
    {
        auto comp_v = boost::add_vertex(comp_g);
        grow_(*p.first, dep_g, comp_v, comp_g, dep_comp_map);

        p.first = find_first_unseen_(++p.first, p.second, dep_comp_map);
    }

    // construct the edges of the component graph
    for(const auto & e : gubg::make_range(boost::edges(dep_g)))
    {
        auto src_it = dep_comp_map.find(boost::source(e, dep_g));
        auto dst_it = dep_comp_map.find(boost::source(e, dep_g));
        MSS(src_it != dep_comp_map.end());
        MSS(dst_it != dep_comp_map.end());

        auto src_v = src_it->second;
        auto dst_v = dst_it->second;
        if (src_v != dst_v)
            boost::add_edge(src_v, dst_v, comp_g);
    }

    MSS_END();
}

}

Menu::Menu()
    : valid_(false)
{

}

const std::list<model::Recipe*> & Menu::topological_order() const
{
    return topological_order_;
}

Result Menu::construct_()
{
    MSS_BEGIN(Result);

    valid_ = false;

    MSS(algo::topological_order(root_recipes_, topological_order_));
    MSS(initialize_process_info_());

    valid_ = true;

    MSS_END();
}


const std::list<model::Recipe*> & Menu::root_recipes() const
{
    return root_recipes_;
}


bool Menu::initialize_process_info_()
{
    MSS_BEGIN(bool);

    // construc the component graph
    CompG component_graph;
    MSS(construct_component_graph_(gubg::make_range(root_recipes_), component_graph));

    // order it topologically
    std::vector<CompV> vertices(boost::num_vertices(component_graph));
    try
    {
        boost::topological_sort(component_graph, vertices.begin());
    }
    catch(boost::not_a_dag)
    {
        MSS(false);
    }

    // vertices is order from can execute first, to should execute last
    for(CompV v : gubg::make_range(boost::vertices(component_graph)))
    {
        // a single graph per component
        const auto & recipe_set = component_graph[v];
        build::GraphPtr ptr = std::make_shared<build::Graph>();

        for(auto * recipe : recipe_set)
            process_info_map_.emplace(recipe, ProcessInfo(ptr));

        topological_build_graph_order_.push_back(ptr);
    }

    MSS_END();
}

} }

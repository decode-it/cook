#include "cook/process/Menu.hpp"
#include "cook/algo/TopologicalOrder.hpp"
#include "cook/algo/DependencyGraph.hpp"
#include "cook/algo/ComponentGraph.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/graph/topological_sort.hpp"
#include <cassert>

namespace cook { namespace process {

Menu::Menu()
    : valid_(false)
{

}

const std::list<model::Recipe*> & Menu::topological_order_recipes() const
{
    return topological_order_;
}

const std::list<build::GraphPtr> & Menu::topological_order_build_graphs() const
{
    return topological_build_graph_order_;
}

const RecipeFilteredGraph * Menu::recipe_filtered_graph(model::Recipe * recipe) const
{
    auto it = recipe_filtered_graphs_.find(recipe);
    return it == recipe_filtered_graphs_.end() ? nullptr : &(it->second);
}

RecipeFilteredGraph * Menu::recipe_filtered_graph(model::Recipe *recipe)
{
    auto it = recipe_filtered_graphs_.find(recipe);
    return it == recipe_filtered_graphs_.end() ? nullptr : &(it->second);
}

Result Menu::construct_()
{
    MSS_BEGIN(Result);

    valid_ = false;

    MSS(algo::make_TopologicalOrder(root_recipes_, topological_order_));
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

    using DepG = boost::adjacency_list<boost::listS, boost::listS, boost::bidirectionalS, model::Recipe *>;
    using CompG = boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS, std::set<model::Recipe *>>;
    using DepV = boost::graph_traits<DepG>::vertex_descriptor;
    using CompV = boost::graph_traits<CompG>::vertex_descriptor;

    // construct a dependency graph
    DepG dep_g;
    std::unordered_map<model::Recipe *, DepV> recipe_dep_map;
    MSS(algo::make_DependencyGraph(gubg::make_range(root_recipes_), dep_g, recipe_dep_map));

    // construct the component graph
    CompG component_graph;
    std::unordered_map<DepV, CompV> dep_comp_map;
    MSS(algo::make_ComponentGraph(dep_g, component_graph, dep_comp_map));

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
            recipe_filtered_graphs_.emplace(recipe, RecipeFilteredGraph(ptr));

        topological_build_graph_order_.push_back(ptr);
    }

    MSS_END();
}

} }

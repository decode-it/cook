#include "cook/process/Menu.hpp"
#include "cook/algo/TopologicalOrder.hpp"
#include "cook/algo/DependencyGraph.hpp"
#include "cook/algo/ComponentGraph.hpp"
#include "gubg/graph/TopologicalSort.hpp"
#include <cassert>

namespace cook { namespace process {

Menu::Menu()
    : valid_(false)
{

}

bool Menu::is_valid() const
{
    return valid_;
}

const std::list<model::Recipe*> & Menu::topological_order_recipes() const
{
    return topological_order_;
}

Result Menu::topological_order_recipes(model::Recipe * root, std::list<model::Recipe*> & result) const
{
    MSS_BEGIN(Result);
    MSS(is_valid());
    MSS(!!root);

    std::unordered_set<model::Recipe *> todo( {root} );

    for (auto it = topological_order_recipes().rbegin(); it != topological_order_recipes().rend(); ++it)
    {
        model::Recipe * cur =*it;
        MSS(!!cur);

        // does this occur in the todo?
        if (todo.erase(cur) == 0)
            continue;

        // add to the list
        result.push_front(cur);

        // and allow the children for processing
        for(model::Recipe * dep : cur->dependencies())
            todo.insert(dep);
    }

    MSS(todo.empty());

    MSS_END();
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

const Menu::DependencyGraph & Menu::dependency_graph() const
{
    return dependency_graph_;
}
const Menu::ComponentGraph & Menu::component_graph() const
{
    return component_graph_;
}

Result Menu::construct_()
{
    MSS_BEGIN(Result);

    valid_ = false;

    using ComponentVertex = gubg::graph::Traits<ComponentGraph::Graph>::vertex_descriptor;

    // build the dependency graph
    MSS(algo::make_DependencyGraph(gubg::make_range(root_recipes()), dependency_graph_.graph, dependency_graph_.translation_map));

    // create the topological order
    MSS(algo::make_TopologicalOrder(dependency_graph().graph, std::back_inserter(topological_order_)));

    // construct the component graph
    MSS(algo::make_ComponentGraph(dependency_graph_.graph, component_graph_.graph, component_graph_.translation_map));

    // try to order the component graph topologically
    std::vector<ComponentVertex> vertices(gubg::graph::num_vertices(component_graph().graph));

    MSS(gubg::graph::construct_topological_order(component_graph().graph, vertices.rbegin()));

    // process the components in topological order
    for(ComponentVertex v : vertices)
    {
        // a single graph per component
        build::GraphPtr ptr = std::make_shared<build::Graph>();

        // and use the graph for every recipe in the component
        const auto & comp = gubg::graph::vertex_label(v, component_graph().graph);
        for(auto * recipe : comp)
            recipe_filtered_graphs_.emplace(recipe, RecipeFilteredGraph(ptr));

        // and assign the build graph order to the topological list
        topological_build_graph_order_.push_back(ptr);
    }

    valid_ = true;

    MSS_END();
}


const std::list<model::Recipe*> & Menu::root_recipes() const
{
    return root_recipes_;
}

void Menu::stream(log::Importance imp) const
{
    auto ss = log::scope("topological_order", imp);
    for (auto r: topological_order_)
    {
        auto ss = log::scope("recipe", imp, [&](auto & n) { n.attr("uri", r->uri()); });
    }
}

} }

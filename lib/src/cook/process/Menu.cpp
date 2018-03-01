#include "cook/process/Menu.hpp"
#include "cook/algo/Recipe.hpp"
#include "boost/graph/adjacency_list.hpp"
#include <cassert>

namespace cook { namespace process {

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

bool Menu::grow_(model::Recipe * seed, build::GraphPtr graph_ptr, const std::unordered_multimap<model::Recipe *, model::Recipe *> & in_edge_map)
{
    MSS_BEGIN(bool);

    std::stack<model::Recipe*>todo;
    todo.push(seed);

    while(!todo.empty())
    {
        model::Recipe * recipe = todo.top();
        todo.pop();

        auto p = process_info_map_.emplace(recipe, ProcessInfo(recipe->uri(), graph_ptr));
        ProcessInfo & pinfo = p.first->second;

        if (!p.second)
            continue;

        // early globbing means our children can share the current graph
        if (recipe->allows_early_globbing())
            for(model::Recipe * dep : recipe->dependencies())
                todo.push(dep);

        // loop over the parents
        for(auto p = in_edge_map.equal_range(recipe); p.first != p.second; ++p.first)
        {
            model::Recipe * parent = p.first->second;
            if (parent->allows_early_globbing())
                todo.push(parent);
        }
    }

    MSS_END();
}

bool Menu::initialize_process_info_()
{
    MSS_BEGIN(bool);

    // get a temporary list of dependency in edges
    std::unordered_multimap<model::Recipe *, model::Recipe *> dep_in_edges;
    for(model::Recipe * recipe : topological_order_)
        for(model::Recipe * dep : recipe->dependencies())
            dep_in_edges.insert(std::make_pair(dep, recipe));

    // assign a single build graph to every component
    {
        build::GraphPtr ptr = std::make_shared<build::Graph>();

        // loop over all recipes
        for(model::Recipe * recipe : topological_order_)
            if (process_info_map_.find(recipe) == process_info_map_.end())
                MSS(grow_(recipe, ptr, dep_in_edges));
    }

    MSS_END();
}

} }

#include "cook/process/Menu.hpp"
#include "cook/algo/Visit.hpp"
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

    // construct the count map and resolve the dependencies
    CountMap in_degree_map;
    MSS(construct_count_map_(in_degree_map), valid_ = false);

    // extract a topological order
    MSS(construct_topological_order_(in_degree_map), valid_ = false);

    MSS(initialize_process_info_());

    valid_ = true;

    MSS_END();
}

Result Menu::construct_topological_order_(CountMap & in_degree_map)
{
    MSS_BEGIN(Result);

    auto initialization_function = [&](auto & stack)
    {
        for(const auto & p : in_degree_map)
            if (p.second == 0)
                stack.push(p.first);
    };

    auto process_function = [&](model::Recipe * recipe, auto & stack)
    {
        MSS_BEGIN(Result);

        MSS(in_degree_map[recipe] == 0);
        topological_order_.push_back(recipe);

        for(model::Recipe * dep : recipe->dependencies())
        {
            auto & count = in_degree_map[dep];
            MSS(count > 0);

            if (--count == 0)
                stack.push(dep);
        }

        MSS_END();
    };

    topological_order_.clear();
    MSS(algo::visit(initialization_function, process_function));

    MSG_MSS(topological_order_.size() == in_degree_map.size(), Error, "The recipes contain cyclic dependencies");

    MSS_END();

}

const std::list<model::Recipe*> & Menu::root_recipes() const
{
    return root_recipes_;
}

Result Menu::construct_count_map_(CountMap & in_degree_map) const
{
    MSS_BEGIN(Result);
    MSS(in_degree_map.empty());

    auto initialization_function = [&](auto & stack)
    {
        for(model::Recipe * recipe : root_recipes_)
        {
            stack.push(recipe);
            in_degree_map[recipe] = 0;
        }
    };

    Result rc;

    auto process_function = [&](model::Recipe * recipe, auto & stack)
    {
        MSS_BEGIN(Result);
        MSS(!!recipe);

        for (const auto & p : recipe->dependency_pairs())
        {
            model::Recipe * dep = p.second;

            if (dep == nullptr)
            {
                rc << MESSAGE(Error, "recipe " << recipe->uri() << " has an unresolved dependency on " << p.first);
            }
            else
            {
                stack.push(dep);
                ++in_degree_map[dep];
            }
        }

        MSS_END();
    };

    MSS(algo::visit(initialization_function, process_function));

    MSS(rc);
    MSS_END();
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

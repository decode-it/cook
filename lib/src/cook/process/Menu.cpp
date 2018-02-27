#include "cook/process/Menu.hpp"
#include "cook/algo/Visit.hpp"
#include "boost/graph/adjacency_list.hpp"
#include <cassert>


namespace cook { namespace process {

Menu::Menu(model::Book * root_book)
    : root_(root_book)
{
    assert(root_);
    assert(root_->is_root());

    if (!root_ || !root_->is_root())
        throw std::logic_error("Expected the root book");
}

Result Menu::construct(const std::list<model::Recipe *> & roots)
{
    MSS_BEGIN(Result);
    CountMap in_degree_map;
    MSS(construct_count_map_(in_degree_map, roots));

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
            auto & count = in_degree_map[recipe];
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

Result Menu::construct_count_map_(CountMap & in_degree_map, const std::list<model::Recipe*> & roots)
{
    MSS_BEGIN(Result);
    MSS(in_degree_map.empty());

    auto initialization_function = [&](auto & stack)
    {
        for(model::Recipe * recipe : roots)
        {
            stack.push(recipe);
            in_degree_map[recipe] = 0;
        }
    };

    auto process_function = [&](model::Recipe * recipe, auto & stack)
    {
        MSS_BEGIN(Result);
        MSS(!!recipe);

        for (const auto & p : recipe->dependency_pairs())
        {
            model::Recipe * dep = p.second;

            MSG_MSS(!!dep, Error, "recipe " << recipe->uri() << " has unresolved dependency on " << p.first);
            stack.push(dep);

            ++in_degree_map[dep];
        }

        MSS_END();
    };

    MSS(algo::visit(initialization_function, process_function));

    MSS(initialize_process_info_());
    MSS_END();
}

bool Menu::grow_(model::Recipe * seed, unsigned int component_identifier, const std::unordered_multimap<model::Recipe *, model::Recipe *> & in_edge_map)
{
    MSS_BEGIN(bool);

    std::stack<model::Recipe*>todo;
    todo.push(seed);

    while(!todo.empty())
    {
        model::Recipe * recipe = todo.top();
        todo.pop();

        auto p = process_info_map_.emplace(recipe, ProcessInfo(recipe->uri()));
        if (!p.second)
        {
            const ProcessInfo & pinfo = p.first->second;
            MSS(pinfo.component_identifier == component_identifier);
            continue;
        }

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

    // fill a color map for components (sharing the same graph
    std::vector<std::shared_ptr<FileDependencyGraph>> dep_graphs;

    {
        unsigned int current_component = 0;

        // loop over all recipes
        for(model::Recipe * recipe : topological_order_)
            if (process_info_map_.find(recipe) == process_info_map_.end())
                MSS(grow_(recipe, current_component++, dep_in_edges));

        // initialize the dependency graphs
        dep_graphs.resize(current_component);
        for(auto & ptr : dep_graphs)
            ptr = std::make_shared<FileDependencyGraph>();
    }

    // initialize the process data per element
    for(model::Recipe * recipe : topological_order_)
    {
        auto it = process_info_map_.find(recipe);
        MSS(it != process_info_map_.end());
        ProcessInfo & info = it->second;

        // set the graph
        MSS(info.component_identifier < dep_graphs.size());
        info.graph = dep_graphs[info.component_identifier];
    }
}


} }



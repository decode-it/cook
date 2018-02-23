#include "cook/algo/DependencyGraph.hpp"
#include "cook/algo/Recipe.hpp"
#include "cook/algo/Visit.hpp"
#include <unordered_map>

using namespace cook::model;

namespace cook { namespace algo {

namespace  {

using CountMap = std::unordered_map<Recipe *, unsigned int>;

template <typename It>
Result fill_count_map(gubg::Range<It> root_recipes, CountMap & count_map)
{
    MSS_BEGIN(Result);

    count_map.clear();

    auto initialize = [&](auto & todo)
    {
        for(Recipe * recipe : root_recipes)
        {
            count_map[recipe] = 0;
            todo.push(recipe);
        }
    };

    auto count_dependencies = [&](Recipe * cur, auto & todo){

        MSS_BEGIN(bool);

        // loop over all the dependencies
        for(model::Recipe * dep : cur->dependencies())
        {
            // dependency should be set or we should not get here
            MSS(!!dep);

            // increase the counter
            ++count_map[dep];
            todo.push(dep);
        }

        MSS_END();
    };
    MSS(visit(initialize, count_dependencies));

    MSS_END();
}

}


void DependencyGraph::clear()
{
    root_recipes_.clear();
    topological_order_.clear();
    result_ = ConstructionResult::InternalError;
}

bool DependencyGraph::topological_suborder(model::Recipe * new_root, const std::list<model::Recipe*> & topological_order, std::list<model::Recipe *> & new_order)
{
    MSS_BEGIN(bool);

    auto it = std::find(topological_order.begin(), topological_order.end(), new_root);
    MSS(it != topological_order.end());

    // all children are more the right
    std::unordered_set<Recipe*> in_tree;
    in_tree.insert(new_root);

    unsigned int inserted = 0;

    for(; it != topological_order.end(); ++it)
    {
        model::Recipe * cur = *it;

        // does this recipe belong to the current subtree?
        if (in_tree.find(cur) == in_tree.end())
            continue;

        new_order.push_back(cur);
        ++inserted;

        for(Recipe * dep : cur->dependencies())
            in_tree.insert(dep);
    }

    // in a valid order we have seen the complete tree
    MSS(inserted == in_tree.size());

    MSS_END();
}

bool DependencyGraph::topological_order(model::Recipe * root, std::list<model::Recipe *> & order) const
{
    MSS_BEGIN(bool);
    MSS(is_acyclic(), order.clear());

    MSS(topological_suborder(root, topological_order_, order));

    MSS_END();
}

const std::list<model::Recipe *> & DependencyGraph::topological_order() const
{
    return topological_order_;
}

Result DependencyGraph::construct_()
{
    MSS_BEGIN(Result);

    // try to resolve all dependencies
    {
        bool all_resolved = true;
        MSS(resolve_recursive_dependencies_(all_resolved));

        if (!all_resolved)
        {
            result_ = ConstructionResult::UnresolvedDependencies;
            MSS_RETURN_OK();
        }
    }

    // try to sort topologically
    {
        bool acyclic = true;
        MSS(sort_topologically_(acyclic));

        if (!acyclic)
        {
            result_ = ConstructionResult::CyclicDependencies;
            MSS_RETURN_OK();
        }
    }

    result_ = ConstructionResult::OK;
    MSS_END();
}

Result DependencyGraph::sort_topologically_(bool & acyclic)
{
    MSS_BEGIN(Result);

    CountMap map;
    MSS(fill_count_map(gubg::make_range(root_recipes_), map));

    // extract the true roots
    {
    auto add_true_roots = [&](auto & stack)
    {
        for(const auto & p : map)
            if (p.second == 0)
                stack.push(p.first);
    };

    auto process_topologically = [&](Recipe * cur, auto & stack)
    {
        MSS_BEGIN(bool);

        MSS(map[cur] == 0);
        topological_order_.push_back(cur);

        // process the children
        for (Recipe * dep: cur->dependencies())
        {
           // decrease the incoming edge counter
            unsigned int & cnt = map[dep];
            MSS(cnt > 0);

            // zero, than we can process it
            if (--cnt == 0)
               stack.push(dep);
        }

        MSS_END();
    };

    MSS(visit(add_true_roots, process_topologically));
    }

    // all edges set to zero ?
    acyclic = std::all_of(RANGE(map), [](const auto & p) { return p.second == 0; });

    // add a warning if not acyclic
    MSG_MSS(acyclic, Warning, "The recipes contain a cyclic dependency");

    MSS_END();
}


bool DependencyGraph::all_dependencies_resolved() const
{
    return (result_ & ConstructionResult::UnresolvedDependencies) == 0;
}

bool DependencyGraph::is_acyclic() const
{
    return (result_ & ConstructionResult::CyclicDependencies) == 0;
}

Result DependencyGraph::resolve_recursive_dependencies_(bool & all_resolved)
{
    MSS_BEGIN(Result);
    all_resolved = true;

    auto initialize = [&](auto & todo)
    {
        for(Recipe * recipe : root_recipes_)
            todo.push(recipe);
    };

    auto dependency_resolver = [&](Recipe * cur, auto & todo)
    {
        MSS_BEGIN(Result);

        MSS(resolve_dependencies(cur, &all_resolved));

        for(Recipe * dep : cur->dependencies())
            if (dep)
                todo.push(dep);

        MSS_END();
    };

    MSS(visit(initialize, dependency_resolver));

    MSS_END();
}

} }


#include "cook/algo/DependencyResolver.hpp"
#include "cook/model/Recipe.hpp"
#include "gubg/mss.hpp"
#include <unordered_set>
#include <unordered_map>
#include <cassert>
#include <stack>

using namespace cook::model;

namespace cook { namespace algo {

namespace  {

bool resolve_relative_dependency(Recipe *& dep, const Uri & uri, Book * book)
{
    MSS_BEGIN(bool);

    dep = nullptr;
    while(book != nullptr)
    {
        MSS(find_recipe(dep, book, uri));
        if (dep)
            MSS_RETURN_OK();

        book = book->parent();
    }

    MSS_END();
}

bool resolve_dependencies(Book * root)
{
    MSS_BEGIN(bool);

    // make sure this is the root book
    MSS(!!root);
    MSS(root->uri().absolute());
    MSS(root->parent() == nullptr);

    std::stack<Book *> todo;
    todo.push(root);

    while(!todo.empty())
    {
        Book * cur = todo.top();
        todo.pop();

        auto resolve_dependency = [&](Recipe * recipe)
        {
            MSS_BEGIN(bool);

            for (const auto & p : recipe->dependencies())
            {
                Recipe * dep = nullptr;
                const Uri & uri = p.first;

                if (uri.absolute())
                    MSS(find_recipe(dep, root, uri));
                else
                    MSS(resolve_relative_dependency(dep, uri, cur));

                MSS(recipe->resolve_dependency(uri, dep));
            }

            MSS_END();
        };

        MSS(cur->each_recipe(resolve_dependency));

        cur->each_book([&](Book * child) {
            todo.push(child); return true;
        });
    }

    MSS_END();
}

}

DependencyResolver::result_type DependencyResolver::fill_count_map_(CountMap & map, const std::list<Recipe *> & recipe_roots)
{
    map.clear();
    std::unordered_set<Recipe *> seen;

    std::stack<Recipe *> todo;
    for(Recipe * recipe: recipe_roots)
    {
        map[recipe] = 0;
        todo.push(recipe);
    }

    // construct a count map and check for unresolved dependencies
    while(!todo.empty())
    {
        Recipe * cur = todo.top();
        todo.pop();

        // already processed ?
        if (!seen.insert(cur).second)
            continue;

        // loop over all the dependencies
        for(const auto & p : cur->dependencies())
        {
            Recipe * dep = p.second;

            // check whether resolved
            if (!dep)
                return unresolved_dependencies;

            // increase the counter
            ++map[dep];
            todo.push(dep);
        }
    }

    return success;
}

DependencyResolver::result_type DependencyResolver::resolve(const std::list<Recipe *> & recipe_roots)
{
    // resolve as much dependencies as possible
    if (!resolve_dependencies(root_))
        return internal_error;

    // fill a count map and check for unresolved dependencies
    CountMap incoming_map;
    {
        result_type res = fill_count_map_(incoming_map, recipe_roots);
        if (res != success)
            return res;
    }

    return topological_sort_(incoming_map);
}

DependencyResolver::result_type DependencyResolver::topological_sort_(CountMap & map)
{
    // clear but reserve the map
    order_.resize(map.size());
    auto out = order_.begin();

    std::stack<Recipe *> todo;
    for(auto & p : map)
        if (p.second == 0)
            todo.push(p.first);

    while(!todo.empty())
    {
        Recipe * cur = todo.top();
        todo.pop();

        assert(map[cur] == 0);
        assert(out < order_.end());
        *out++ = cur;

        // process the children
        for (const auto & p : cur->dependencies())
        {
            Recipe * dep = p.second;

            // decrease the incoming edge counter
            unsigned int & cnt = map[dep];
            assert(cnt > 0);

            // zero, than we can process it
            if (--cnt == 0)
                todo.push(dep);
        }
    }

    // all edges set to zero ?
    for(auto & p : map)
        if (p.second != 0)
            return cyclic_dependencies;

    return success;
}


const std::vector<model::Recipe *> & DependencyResolver::topological_order() const
{
    return order_;
}

std::vector<model::Recipe *> DependencyResolver::topological_suborder(model::Recipe * subroot) const
{
    {
        auto it = std::find(order_.begin(), order_.end(), subroot);
        if (it == order_.end())
            return {};
    }

    // process the subgraph
    std::unordered_set<Recipe *> seen;
    std::stack<Recipe *> todo;
    todo.push(subroot);

    while(!todo.empty())
    {
        Recipe * cur = todo.top();
        todo.pop();

        while(!seen.insert(cur).second)
            continue;

        for (const auto & p : cur->dependencies())
            todo.push(p.second);
    }

    std::vector<model::Recipe *> subset;
    for(Recipe * recipe : order_)
        if (seen.count(recipe) > 0)
        subset.push_back(recipe);

    return subset;
}

} }

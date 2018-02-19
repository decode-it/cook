#include "cook/model/DependencyGraph.hpp"
#include "cook/model/Book.hpp"
#include <stack>
#include <unordered_set>

namespace cook { namespace model {

DependencyGraph::DependencyGraph(Recipe *root_vertex)
    : root_vertex_(root_vertex)
{
}

bool DependencyGraph::resolve(bool & success)
{
    MSS_BEGIN(bool);
    success = false;

    std::unordered_set<Recipe *> visited;
    network_.clear();

    MSS(!!root_vertex_);
    MSS(!!root_vertex_->book());


    std::stack<Recipe *> todo;
    todo.push(root_vertex_);

    // resolve all the dependencies (if possible)
    while(!todo.empty())
    {
        Recipe * recipe = todo.top();
        todo.pop();

        // already present ?
        if (!visited.insert(recipe).second)
            continue;

        // process all dependencies
        for (const auto & dep : recipe->dependencies())
        {
            const Uri & uri = dep.first;
            MSS(uri.has_name());

            // try to resolve
            Recipe * dependency = nullptr;
            if(uri.absolute())
                dependency = resolve_absolute_dependency_(recipe->book(), uri);
            else
                dependency = resolve_relative_dependency_(recipe->book(), uri);

            // not found, signal but still continue
            if (!dependency)
                success = false;
            else
                MSS(recipe->resolve_dependency(uri, dependency));

            todo.push(dependency);
        }
    }

    MSS_END();
}

std::vector<Recipe *> DependencyGraph::topological_order()
{
    std::vector<Recipe *> result (network_.size());
    unsigned int position = 0;
    network_.each_vertex<gubg::network::Direction::Forward>([&](Recipe & recipe) { result[position] = &recipe; return true; });

    return result;
}

Recipe * DependencyGraph::resolve_relative_dependency_(Book * book, const Uri & uri)
{
    // go to the root
    assert(!!book);
    while(book->parent() != nullptr)
        book = book->parent();

    // and try to find the recipe here
    Recipe * result = nullptr;
    find_recipe(result, book, uri);
    return result;
}

Recipe * DependencyGraph::resolve_absolute_dependency_(Book * book, const Uri & uri)
{
    // start from book the most towards the leaf
    while(book != nullptr)
    {
        // try to find the recipe
        Recipe * result = nullptr;
        find_recipe(result, book, uri);
        if (result)
            return result;

        // and travel towards the root
        book = book->parent();
    }

    return nullptr;
}

} }



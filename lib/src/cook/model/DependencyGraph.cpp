#include "cook/model/DependencyGraph.hpp"
#include "cook/model/Book.hpp"
#include <stack>
#include <unordered_set>

namespace cook { namespace model {

DependencyGraph::DependencyGraph(Recipe *root_vertex)
    : root_vertex_(root_vertex)
{
}

bool DependencyGraph::resolve()
{
    MSS_BEGIN(bool);

    std::unordered_set<Recipe *> visited;
    network_.clear();

    MSS(!!root_vertex_);
    MSS(!!root_vertex_->book());


    std::stack<Recipe *> todo;
    todo.push(root_vertex_);

    // resolve all the dependencies
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
            // try to resolve
            Recipe * dependency = nullptr;
            MSS(resolve_dependency_(dependency, recipe, dep));
            MSS(!!dependency, std::cout << "Unable to resolve dependency for " << recipe->uri() << " on " << dep.string());

            // add an edge
            MSS(network_.add_edge(recipe, dependency));
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

bool DependencyGraph::resolve_dependency_(Recipe *&dependency, const Recipe * recipe, const Recipe::Dependency &uri)
{
    MSS_BEGIN(bool);

    dependency = nullptr;


    MSS(uri.has_name());
    MSS(recipe->uri().absolute());

    if (uri.absolute())
    {
        // go to the root
        Book * root = recipe->book();
        {
            MSS(!!root);
            while(root->parent() != nullptr)
                root = root->parent();
        }

        // and try to find the recipe here
        MSS(find_recipe(dependency, root, uri));
    }
    else
    {
        // start from book the most towards the leaf
        Book * book = recipe->book();
        while(book != nullptr && dependency == nullptr)
        {
            // try to find the recipe
            MSS(find_recipe(dependency, book, uri));

            // and travel towards the root
            book = book->parent();
        }
    }

    MSS_END();
}

} }



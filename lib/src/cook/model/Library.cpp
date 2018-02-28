#include "cook/model/Library.hpp"
#include "cook/model/Recipe.hpp"
#include <stack>
#include <cassert>

namespace cook{ namespace model {

std::list<Recipe *> Library::list_all_recipes() const
{
    std::list<Recipe *> result;

    std::stack<const Book *> todo;
    todo.push(root());

    while(!todo.empty())
    {
        const Book * book = todo.top();
        todo.pop();

        result.insert(result.end(), RANGE(book->recipes()));
        for(const model::Book * b : book->books())
            todo.push(b);
    }

    return result;
}

Result Library::find_first_relative_(Recipe *& recipe, const Uri & uri, Book * root) const
{
    MSS_BEGIN(Result);

    recipe = nullptr;

    MSS(!!root);
    MSS(!uri.absolute() || root->is_root());

    while(root != nullptr)
    {
        MSS(find_recipe(recipe, uri, root));
        if (recipe)
            MSS_RETURN_OK();

        root = root->parent();
    }

    MSS_END();
}

Result Library::resolve(bool * all_resolved) const
{
    MSS_BEGIN(Result);

    bool unresolved_dependency_found = false;

    std::list<Recipe *> recipes = list_all_recipes();
    for(Recipe * recipe : recipes)
    {
        for(const auto & p : recipe->dependency_pairs())
        {
            Recipe * dep = p.second;
            const Uri & uri = p.first;
            if (dep != nullptr)
                continue;

            if (uri.absolute())
                MSS(find_recipe(dep, uri));
            else
                MSS(find_first_relative_(dep, uri, recipe->parent()));

            if (dep == nullptr)
            {
                unresolved_dependency_found = true;
                MSS_RC << MESSAGE(Info, "Recipe " << recipe->uri() << " has an unresolved dependency on " << uri);
            }
            else
            {
                MSS(recipe->resolve_dependency(uri, dep));
            }
        }
    }

    if (all_resolved)
        *all_resolved = !unresolved_dependency_found;

    MSS_END();
}



Result Library::find_recipe(Recipe *& result, const Uri & uri, Book * relative_root) const
{
    MSS_BEGIN(Result);
    relative_root = relative_root == nullptr ? root() : relative_root;
    MSG_MSS(!uri.absolute() || relative_root->is_root(), Error, "The supplied uri " << uri << " is absolute, but the relative root book " << relative_root->uri() << " is not");

    MSS(Book::find_relative(result, uri.as_relative(), relative_root));
    MSS_END();
}

Result Library::goc_recipe(Recipe *& result, const Uri & uri, Book * relative_root)
{
    MSS_BEGIN(Result);
    relative_root = relative_root == nullptr ? root() : relative_root;
    MSG_MSS(!uri.absolute() || relative_root->is_root(), Error, "The supplied uri " << uri << " is absolute, but the relative root book " << relative_root->uri() << " is not");

    MSS(Book::goc_relative(result, uri.as_relative(), relative_root));
    MSS_END();
}

Result Library::find_book(Book *& result, const Uri & uri, Book * relative_root) const
{
    MSS_BEGIN(Result);
    relative_root = relative_root == nullptr ? root() : relative_root;
    MSG_MSS(!uri.absolute() || relative_root->is_root(), Error, "The supplied uri " << uri << " is absolute, but the relative root book " << relative_root->uri() << " is not");

    MSS(Book::find_relative(result, uri.as_relative(), relative_root));
    MSS_END();
}

Result Library::goc_book(Book *& result, const Uri & uri, Book * relative_root)
{
    MSS_BEGIN(Result);
    relative_root = relative_root == nullptr ? root() : relative_root;
    MSG_MSS(!uri.absolute() || relative_root->is_root(), Error, "The supplied uri " << uri << " is absolute, but the relative root book " << relative_root->uri() << " is not");

    MSS(Book::goc_relative(result, uri.as_relative(), relative_root));
    MSS_END();
}


} }

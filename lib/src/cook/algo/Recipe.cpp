#include "cook/algo/Recipe.hpp"
#include "cook/algo/Book.hpp"
#include "gubg/mss.hpp"

using namespace cook::model;

namespace cook { namespace algo {

namespace  {

bool find_absolute_recipe(Recipe *& result, Book * book, const Uri & uri)
{
    MSS_BEGIN(bool);

    while(!!book->parent())
        book = book->parent();

    MSS(find_recipe(result, book, uri));

    MSS_END();
}


bool find_relative_recipe(Recipe *& result, Book * book, const Uri & uri)
{
    MSS_BEGIN(bool);

    result = nullptr;

    while(!!book  && result == nullptr)
    {
        MSS(find_recipe(result, book, uri));
        book = book->parent();
    }

    MSS_END();
}

}

Result resolve_dependencies(Recipe * recipe, bool * all_resolved)
{
    MSS_BEGIN(Result);
    MSS(!!recipe);

    Result rc;

    for(const auto & p : recipe->dependency_pairs())
    {
        const Uri & uri = p.first;
        MSS(uri.has_name());

        Recipe * result = nullptr;

        if (uri.absolute())
            MSS(find_absolute_recipe(result, recipe->parent(), uri));
        else
            MSS(find_relative_recipe(result, recipe->parent(), uri));

        MSG_MSS(!!result, Warning, "could not resolve dependency '" << uri << "' for recipe " << recipe->uri());

        if (result)
            MSS(recipe->resolve_dependency(uri, result));
        else if (all_resolved)
            *all_resolved = false;
    }

    MSS(rc);

    MSS_END();
}

} }


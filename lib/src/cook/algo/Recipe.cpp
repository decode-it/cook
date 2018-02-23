#include "cook/algo/Recipe.hpp"
#include "cook/algo/Book.hpp"
#include "gubg/mss.hpp"

using namespace cook::model;

namespace cook { namespace algo {

namespace  {

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

Result resolve_dependency(model::Recipe *&result, const model::Uri & uri, model::Book * current_book, model::Book * root_book)
{
    MSS_BEGIN(Result);
    MSS(!!current_book);
    MSS(!!root_book);
    MSS(root_book->is_root());

    if (uri.absolute())
        MSS(find_recipe(result, root_book, uri));
    else
        MSS(find_relative_recipe(result, current_book, uri));

    MSS_END();
}

} }


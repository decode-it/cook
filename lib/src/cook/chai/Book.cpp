#include "cook/chai/Book.hpp"
#include "cook/chai/Recipe.hpp"
#include "cook/chai/Context.hpp"
#include "cook/chai/mss.hpp"
#include "cook/Log.hpp"

namespace cook { namespace chai {

Book::Book(model::Book * book, Context * context)
    : book_(book),
      context_(context),
      data_(from_any(book->user_data()))
{
}

Book Book::book(const std::string & uri_str)
{
    CHAI_MSS_BEGIN();

    auto ss = log::scope("goc book", [&](auto & n) {n.attr("parent uri", book_->uri()).attr("uri", uri_str); });

    model::Uri uri;
    CHAI_MSS(model::Uri::book_uri(uri_str, uri));

    model::Book * subbook = nullptr;
    CHAI_MSS(model::Book::goc_relative(subbook, uri, book_));

    return Book(subbook, context_);
}

void Book::book(const std::string & uri_str, const BookFunctor &functor)
{
    CHAI_MSS_BEGIN();

    auto ss = log::scope("goc book", [&](auto & n) {n.attr("parent uri", book_->uri()).attr("uri", uri_str); });

    model::Uri uri;
    CHAI_MSS(model::Uri::book_uri(uri_str, uri));

    model::Book * subbook = nullptr;
    CHAI_MSS(model::Book::goc_relative(subbook, uri, book_));

    functor(Book(subbook, context_));
}

Recipe Book::recipe(const std::string & uri_str, const std::string & type_str)
{
    CHAI_MSS_BEGIN();

    model::Uri uri;
    CHAI_MSS(model::Uri::recipe_uri(uri_str, uri));

    model::Recipe * recipe = nullptr;
    CHAI_MSS(model::Book::goc_relative(recipe, uri, book_));

    using T = TargetType;

    T type = T::Undefined;
    if (false) {}
    else if (type_str == "executable")      { type = T::Executable; }
    else if (type_str == "library")         { type = T::Archive; }
    else if (type_str == "shared_library")  { type = T::SharedLibrary; }
    else if (type_str == "script")          { type = T::Script; }

    Recipe r(recipe);
    r.set_type(type);
    r.set_working_directory(context_->current_working_directory().string());

    return r;
}

void Book::recipe(const std::string & uri_str, const RecipeFunctor &functor)
{
    recipe(uri_str, "", functor);
}

void Book::recipe(const std::string & uri_str, const std::string & type_str, const RecipeFunctor& functor)
{
    functor(recipe(uri_str, type_str));
}

} }

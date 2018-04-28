#include "cook/chai/Book.hpp"
#include "cook/chai/Recipe.hpp"
#include "cook/chai/Context.hpp"
#include "cook/chai/mss.hpp"
#include "cook/Log.hpp"

namespace cook { namespace chai {

Book::Book(model::Book * book, const Context *context)
    : book_(book),
      context_(context),
      data_(from_any(book->user_data()))
{
}

Book Book::book(const std::string & uri_str)
{
    CHAI_MSS_BEGIN();

    auto ss = log::scope("goc book", [&](auto & n) {n.attr("parent uri", book_->uri()).attr("uri", uri_str); });

    model::Uri uri(uri_str);
    model::Book * subbook = nullptr;
    CHAI_MSS(model::Book::goc_relative(subbook, uri, book_));

    return Book(subbook, context_);
}

void Book::book(const std::string & uri_str, const BookFunctor &functor)
{
    CHAI_MSS_BEGIN();

    auto ss = log::scope("goc book", [&](auto & n) {n.attr("parent uri", book_->uri()).attr("uri", uri_str); });

    model::Uri uri(uri_str);
    model::Book * subbook = nullptr;
    CHAI_MSS(model::Book::goc_relative(subbook, uri, book_));

    functor(Book(subbook, context_));
}

Recipe Book::recipe(const std::string & uri_str, const std::string & type_str)
{
    CHAI_MSS_BEGIN();

    model::Uri uri(uri_str);
    CHAI_MSS_MSG(!uri.path().empty(), Error, "The recipe uri can not be empty");

    model::Recipe * recipe = nullptr;
    CHAI_MSS(model::Book::goc_relative(recipe, uri, book_));

    Recipe r(recipe, context_);

    if (false) {}
    else if (type_str == "executable")      { r.set_type(TargetType::Executable); }
    else if (type_str == "library")         { r.set_type(TargetType::Archive); }
    else if (type_str == "shared_library")  { r.set_type(TargetType::SharedLibrary); }
    else if (type_str == "script")          { r.set_type(TargetType::Script); }

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

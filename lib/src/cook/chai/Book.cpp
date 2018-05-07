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

Book Book::book(const model::Uri &uri)
{
    CHAI_MSS_BEGIN();

    auto ss = log::scope("goc book", [&](auto & n) {n.attr("parent uri", book_->uri()).attr("uri", uri); });

    model::Book * subbook = nullptr;
    CHAI_MSS(model::Book::goc_relative(subbook, uri, book_));

    return Book(subbook, context_);
}

void Book::book(const model::Uri & uri, const BookFunctor &functor)
{
    functor(book(uri));
}

bool Book::has_recipe(const model::Uri & uri) const
{
    CHAI_MSS_BEGIN();

    CHAI_MSS_MSG(!uri.path().empty(), Error, "The recipe uri can not be empty");

    model::Recipe * recipe = nullptr;
    CHAI_MSS(model::Book::find_relative(recipe, uri, book_));

    return !!recipe;
}

Recipe Book::recipe(const model::Uri & uri, const std::string & type_str)
{
    CHAI_MSS_BEGIN();

    CHAI_MSS_MSG(!uri.path().empty(), Error, "The recipe uri can not be empty");

    // check whether this book already exists
    model::Recipe * recipe = nullptr;
    bool already_existing = false;

    {
        CHAI_MSS(model::Book::find_relative(recipe, uri, book_));
        if (recipe)
            already_existing = true;
        else
            CHAI_MSS(model::Book::goc_relative(recipe, uri, book_));
    }

    Recipe r(recipe, context_);

    if (false) {}
    else if (type_str == "executable")      { r.set_type(TargetType::Executable); }
    else if (type_str == "library")         { r.set_type(TargetType::Archive); }
    else if (type_str == "shared_library")  { r.set_type(TargetType::SharedLibrary); }
    else if (type_str == "script")          { r.set_type(TargetType::Script); }

    if (!already_existing)
        r.set_working_directory(context_->current_working_directory().string());

    return r;
}

void Book::recipe(const model::Uri & uri, const RecipeFunctor &functor)
{
    recipe(uri, std::string(), functor);
}

void Book::recipe(const model::Uri & uri, const std::string & type_str, const RecipeFunctor& functor)
{
    functor(recipe(uri, type_str));
}

} }

#include "cook/chai/Book.hpp"
#include "cook/chai/Recipe.hpp"
#include "cook/chai/Context.hpp"
#include "cook/Log.hpp"

namespace cook { namespace chai {

Book::Book(model::Book * book, Context * context, Logger * logger)
    : book_(book),
      context_(context),
      logger_(logger),
      data_(from_any(book->user_data()))
{
}

void Book::book(const std::string & uri_str, const std::function<void (Book)> &functor)
{
    auto ss = log::scope("goc book", [&](auto & n) {n.attr("parent uri", book_->uri()).attr("uri", uri_str); });

    std::pair<model::Uri, bool> uri = model::Uri::book_uri(uri_str);
    if (!uri.second)
        logger_->log(Message::Error, "Bad uri");

    model::Book * subbook = nullptr;
    Result rc = model::Book::goc_relative(subbook, uri.first, book_);
    if (!rc)
        logger_->log(rc);

    functor(Book(subbook, context_, logger_));
}


void Book::recipe_2(const std::string & uri_str, const std::function<void (Recipe)> &functor)
{
    recipe_3(uri_str, "", functor);
}

void Book::recipe_3(const std::string & uri_str, const std::string & type_str, const std::function<void (Recipe)> & functor)
{
    std::pair<model::Uri, bool> uri = model::Uri::recipe_uri(uri_str);
    if (!uri.second)
        logger_->log(Message::Error, "Bad uri");

    model::Recipe * recipe = nullptr;
    Result rc = model::Book::goc_relative(recipe, uri.first, book_);
    if (!rc)
        logger_->log(rc);

    cook::Type type = Type::Undefined;
    if (false) {}
    else if (type_str == "executable") { type = Type::Executable; }
    else if (type_str == "library") { type = Type::Library; }

    {
        Recipe r(recipe, context_, logger_);
        r.set_type(type);
        r.set_working_directory(context_->current_working_directory().string());
        functor(r);
    }
}

} }

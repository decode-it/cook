#include "cook/chai/Book.hpp"
#include "cook/chai/Runner.hpp"
#include "cook/chai/Recipe.hpp"
#include "gubg/chai/Module.hpp"

namespace cook { namespace chai {

Book::Book(model::Book * book, Logger *logger)
    : book_(book),
      logger_(logger)
{
}

void Book::book(const std::string & uri_str, const std::function<void (Book)> &functor)
{
    std::pair<model::Uri, bool> uri = model::Uri::book_uri(uri_str);
    if (!uri.second)
        logger_->LOG(Error, "Bad uri");

    model::Book * subbook = nullptr;
    if (!model::goc_book(subbook, book_, uri.first))
        logger_->LOG(Error, "Error");

    functor(Book(subbook, logger_));
}


void Book::recipe_2(const std::string & uri_str, const std::function<void (Recipe)> &functor)
{
    recipe_3(uri_str, "", functor);

}

void Book::recipe_3(const std::string & uri_str, const std::string & type_str, const std::function<void (Recipe)> & functor)
{
    std::pair<model::Uri, bool> uri = model::Uri::recipe_uri(uri_str);
    if (!uri.second)
        logger_->LOG(Error, "Bad uri");

    model::Recipe * recipe = nullptr;
    if (!model::goc_recipe(recipe, book_, uri.first))
        logger_->LOG(Error, "Error");

    cook::Type type = Type::Undefined;
    if (false) {}
    else if (type_str == "executable") { type = Type::Executable; }
    else if (type_str == "library") { type = Type::Library; }

    {
        Recipe r(recipe, logger_);
        r.set_type(type);
        functor(r);
    }
}

gubg::chai::ModulePtr book_module()
{
    gubg::chai::ModulePtr ptr = std::make_unique<chaiscript::Module>();

    ptr->add(chaiscript::user_type<Book>(), "Book");
    ptr->add(chaiscript::fun(&Book::book), "book");
    ptr->add(chaiscript::fun(&Book::recipe_2), "recipe");
    ptr->add(chaiscript::fun(&Book::recipe_3), "recipe");

    return ptr;
}

} }

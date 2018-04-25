#ifndef HEADER_cook_chai_Book_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Book_hpp_ALREADY_INCLUDED

#include "cook/chai/UserData.hpp"
#include "cook/model/Book.hpp"
#include "cook/Logger.hpp"
#include <functional>

namespace cook { namespace chai {

class Context;

struct Recipe;

struct Book
{
    using RecipeFunctor = std::function<void (Recipe)>;
    using BookFunctor = std::function<void (Book)>;

    Book(model::Book * book, Context * context);

    Book book(const std::string & uri_str);
    void book(const std::string & uri_str, const BookFunctor &functor);
    void recipe(const std::string & uri_str, const RecipeFunctor & functor);
    void recipe(const std::string & uri_str, const std::string & type, const RecipeFunctor & functor);
    Recipe recipe(const std::string & uri_str, const std::string & type = std::string());
    const model::Uri & uri() const { return book_->uri(); }

    UserData data() const { return data_; }

private:
    model::Book * book_;
    Context * context_;
    UserData data_;
};

} }

#endif

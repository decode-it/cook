#ifndef HEADER_cook_chai_Book_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Book_hpp_ALREADY_INCLUDED

#include "cook/chai/UserData.hpp"
#include "cook/model/Book.hpp"
#include "cook/Logger.hpp"
#include <functional>

namespace cook { namespace chai {

class Context;
class Recipe;

class Book
{
public:
    using RecipeFunctor = std::function<void (Recipe)>;
    using BookFunctor = std::function<void (Book)>;

    Book(model::Book * book, const Context * context);

    Book book(const model::Uri & uri);
    void book(const model::Uri & uri, const BookFunctor &functor);
    bool has_recipe(const model::Uri & uri) const;
    void recipe(const model::Uri & uri, const RecipeFunctor & functor);
    void recipe(const model::Uri & uri, const std::string & type, const RecipeFunctor & functor);
    Recipe recipe(const model::Uri & uri, const std::string & type = std::string());
    const model::Uri & uri() const { return book_->uri(); }

    UserData data() const { return data_; }

private:
    model::Book * book_;
    const Context * context_;
    UserData data_;
};

} }

#endif

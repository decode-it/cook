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
    Book(model::Book * book, Context * context, Logger * logger);

    void book(const std::string & uri_str, const std::function<void (Book)> &functor);
    void recipe_2(const std::string & uri_str, const std::function<void (Recipe)> & functor);
    void recipe_3(const std::string & uri_str, const std::string & type, const std::function<void (Recipe)> & functor);

    UserData data() const { return data_; }

private:
    model::Book * book_;
    Context * context_;
    Logger * logger_;
    UserData data_;
};

} }

#endif

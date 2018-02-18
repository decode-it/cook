#ifndef HEADER_cook_chai_Book_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Book_hpp_ALREADY_INCLUDED

#include "cook/model/Book.hpp"
#include "gubg/chai/ModuleFwd.hpp"
#include <functional>

namespace cook { namespace chai {

struct Recipe;
struct Runner;

struct Book
{
    Book(model::Book * book, Runner * runner);

    void book(const std::string & uri_str, const std::function<void (Book)> &functor);
    void recipe_2(const std::string & uri_str, const std::function<void (Recipe)> & functor);
    void recipe_3(const std::string & uri_str, const std::string & type, const std::function<void (Recipe)> & functor);

    model::Book * book_;
    Runner * runner_;
};

gubg::chai::ModulePtr book_module();

} }

#endif

#ifndef HEADER_cook_model_Library_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Library_hpp_ALREADY_INCLUDED

#include "cook/model/Book.hpp"
#include "cook/Result.hpp"

namespace cook { namespace model {

struct Library
{
    Book * root() const { return &root_; }

    std::list<Recipe *> list_all_recipes() const;
    Result resolve(bool * all_resolved = nullptr) const;

    Result find_recipe(Recipe *& result, const Uri & uri, Book * relative_root = nullptr) const;
    Result goc_recipe(Recipe *& result, const Uri & uri, Book * relative_root = nullptr);
    Result find_book(Book *& result, const Uri & uri, Book * relative_root = nullptr) const;
    Result goc_book(Book *& result, const Uri & uri, Book * relative_root = nullptr);


private:
    Result find_first_relative_(Recipe *& resipt, const Uri & uri, Book * root) const;

    mutable Book root_;
};




} }

#endif

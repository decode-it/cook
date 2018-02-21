#ifndef HEADER_cook_model_Book_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Book_hpp_ALREADY_INCLUDED

#include "cook/model/Element.hpp"
#include <memory>
#include <map>

namespace cook { namespace model {

class Recipe;

class Book : public Element
{
public:
    Book();
    explicit Book(const Uri & uri);

    Book & goc_book(const Part & part);
    Recipe & goc_recipe(const Part & part);

    Book * find_book(const Part & part) const;
    Recipe * find_recipe(const Part & part) const;

    bool is_root() const;

    template <typename Functor>
    bool each_book(Functor && f) const
    {
        for(const auto & p : subbooks_)
            if (!f(p.second.get()))
                return false;

        return true;
    }

    template <typename Functor>
    bool each_recipe(Functor && f) const
    {
        for(const auto & p : recipes_)
            if (!f(p.second.get()))
                return false;

        return true;
    }


private:
    Book(const Book &) = delete;
    Book & operator=(const Book &) = delete;
    Book(Book &&) = default;
    Book & operator=(Book &&) = delete;

    std::map<Part, std::shared_ptr<Book> > subbooks_;
    std::map<Part, std::shared_ptr<Recipe>> recipes_;
};

bool find_book(Book *& result, Book * book, const Uri & uri);
bool find_recipe(Recipe *& result, Book * book, const Uri & uri);
bool goc_book(Book *& result, Book * book, const Uri & uri);
bool goc_recipe(Recipe *& result, Book * book, const Uri & uri);

} }


#endif // BOOK_HPP

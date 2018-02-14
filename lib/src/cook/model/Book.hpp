#ifndef HEADER_cook_model_Book_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Book_hpp_ALREADY_INCLUDED

#include "cook/model/Uri.hpp"
#include <memory>
#include <map>

namespace cook { namespace model {

class Recipe;

class Book
{
public:
    Book();

    Book & goc_book(const Part & part);
    Recipe & goc_recipe(const Part & part);

private:
    Book(const Uri & uri);
    Book(const Book &) = delete;
    Book & operator=(const Book &) = delete;
    Book(Book &&) = default;
    Book & operator=(Book &&) = delete;

    Uri uri_;
    std::map<Part, std::shared_ptr<Book> > subbooks_;
    std::map<Part, std::shared_ptr<Recipe>> recipes_;
};

} }


#endif // BOOK_HPP

#ifndef HEADER_cook_model_Book_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Book_hpp_ALREADY_INCLUDED

#include "cook/model/Element.hpp"
#include "cook/Result.hpp"
#include "boost/iterator/transform_iterator.hpp"
#include <memory>
#include <map>

namespace cook { namespace model {

class Recipe;

class Book : public Element
{
    using BookMap = std::map<Part, std::shared_ptr<Book>>;
    using RecipeMap = std::map<Part, std::shared_ptr<Recipe>>;

public:
    struct ExtractPointer
    {
        model::Book * operator()(const BookMap::value_type & p) const       { return p.second.get(); }
        model::Recipe * operator()(const RecipeMap::value_type & p) const   { return p.second.get(); }
    };

    using BookIterator = boost::transform_iterator<ExtractPointer, BookMap::const_iterator>;
    using RecipeIterator = boost::transform_iterator<ExtractPointer, RecipeMap::const_iterator>;

    Book();
    explicit Book(const Uri & uri);

    Book & goc_book(const Part & part);
    Recipe & goc_recipe(const Part & part);
    Book * find_book(const Part & part) const;
    Recipe * find_recipe(const Part & part) const;

    static Result find_relative(Recipe *& result, const Uri & uri, Book * relative);
    static Result find_relative(Book *& result, const Uri & uri, Book * relative);
    static Result goc_relative(Recipe *& result, const Uri & uri, Book * relative);
    static Result goc_relative(Book *& result, const Uri & uri, Book * relative);




    bool is_root() const;

    gubg::Range<BookIterator> books() const;
    gubg::Range<RecipeIterator> recipes() const;


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

    BookMap subbooks_;
    RecipeMap recipes_;
};



} }


#endif // BOOK_HPP

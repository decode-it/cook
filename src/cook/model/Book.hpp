#ifndef HEADER_cook_model_Book_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Book_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"
#include "gubg/OnlyOnce.hpp"
#include <map>
#include <sstream>

namespace cook { namespace model { 

    class Book
    {
    public:
        using BookPerName = std::map<std::string, Book>;
        using RecipePerName = std::map<std::string, Recipe>;

        Book() {}
        Book(std::string name): name_(name) {}

        const std::string &name() const {return name_;}

        Book &goc_book(std::string name)
        {
            auto &book = book_per_name_[name];
            book.name_ = name;
            return book;
        }

        Recipe *create_recipe(std::string name)
        {
            if (recipe_per_name_.count(name) > 0)
                return nullptr;
            auto &recipe = recipe_per_name_[name];
            recipe.name_ = name;
            return &recipe;
        }

        const BookPerName &book_per_name() const {return book_per_name_;}
        const RecipePerName &recipe_per_name() const {return recipe_per_name_;}
        void stream(std::ostream &os) const
        {
            os << "name: " << name_ << ", nr books: " << book_per_name_.size() << ", nr recipes: " << recipe_per_name_.size() << std::endl;
        }

    private:
        std::string name_;
        BookPerName book_per_name_;
        RecipePerName recipe_per_name_;
    };

    using BookPath = std::vector<Book*>;
    using ConstBookPath = std::vector<const Book*>;

    inline void name(std::ostream &os, const ConstBookPath &path)
    {
        gubg::OnlyOnce skip;
        for (auto ptr: path)
        {
            if (skip())
                continue;
            os << "/" << ptr->name();
        }
    }
    inline void name(std::ostream &os, const ConstBookPath &path, const Recipe *recipe)
    {
        name(os, path);
        if (!!recipe)
            os << "." << recipe->name();
    }

} } 

#endif

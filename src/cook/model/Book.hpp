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
            os << name_;
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

    class BookStack
    {
    public:
        BookStack()
        {
            path_.push_back(&root_);
        }

        Recipe *recipe() {return recipe_;}

        void push(const std::string &name)
        {
            Book &book = path_.back()->goc_book(name);
            path_.push_back(&book);
        }
        void pop()
        {
            path_.pop_back();
        }

        bool create_recipe(const std::string &name)
        {
            Book &book = *path_.back();
            recipe_ = book.create_recipe(name);
            return !!recipe_;
        }
        void close_recipe()
        {
            recipe_ = nullptr;
        }

        template <typename Ftor>
        void each(Ftor ftor) const
        {
            ConstBookPath path = {&root_};
            each_(ftor, path);
        }

        void stream(std::ostream &os) const
        {
            os << "Library" <<std::endl;
            each([&](const ConstBookPath &path, const Recipe *recipe){
                    os << (!!recipe ? "Recipe: " : "Book: "); name(os, path, recipe); os << std::endl;
                    });
        }

    private:
        template <typename Ftor>
        static void each_(Ftor ftor, ConstBookPath &path)
        {
            for (const auto &p: path.back()->book_per_name())
            {
                const Book &book = p.second;
                path.push_back(&book);
                ftor(path, nullptr);
                for (const auto &p: book.recipe_per_name())
                {
                    ftor(path, &p.second);
                }
                each_(ftor, path);
                path.pop_back();
            }
        }

        Book root_{"ROOT_BOOK"};
        BookPath path_;
        Recipe *recipe_ = nullptr;
    };

} } 

#endif

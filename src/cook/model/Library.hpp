#ifndef HEADER_cook_model_Library_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Library_hpp_ALREADY_INCLUDED

#include "cook/model/Book.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace model { 

    class Library
    {
    public:
        Library()
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

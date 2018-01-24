#ifndef HEADER_cook_model_Book_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Book_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"
#include <map>
#include <sstream>

namespace cook { namespace model { 

    class Book
    {
    public:
        using BookPerName = std::map<std::string, Book>;
        using RecipePerName = std::map<std::string, Recipe>;
        using ScriptFns = std::vector<std::filesystem::path>;

        std::string uri_hr() const {return uri_.str();}

        std::string name() const {return uri_.last_path();}
        std::string display_name() const
        {
            if (display_name_.empty())
                return name();
            return display_name_;
        }

        void set_display_name(const std::string &value)
        {
            display_name_ = value;
        }

        void set_script_filename(const std::string &value)
        {
            script_fns_.push_back(value);
        }

        const ScriptFns &script_filenames() const {return script_fns_;}

        Book &goc_book(const std::string &name)
        {
            auto &book = book_per_name_[name];
            book.uri_ = uri_;
            book.uri_.add_path_part(name);
            return book;
        }

        Recipe *create_recipe(const std::string &name)
        {
            auto p = recipe_per_name_.emplace(name, uri_);
            if (!p.second)
                //Recipe already exists
                return nullptr;
            auto &recipe = p.first->second;
            recipe.set_name(name);
            return &recipe;
        }
        Recipe *get_recipe(const std::string &name)
        {
            auto p = recipe_per_name_.find(name);
            if (p == recipe_per_name_.end())
                return nullptr;
            return &p->second;
        }

        const BookPerName &book_per_name() const {return book_per_name_;}
        BookPerName &book_per_name() {return book_per_name_;}
        const RecipePerName &recipe_per_name() const {return recipe_per_name_;}
        RecipePerName &recipe_per_name() {return recipe_per_name_;}
        void stream(std::ostream &os) const
        {
            os << "name: " << name() << ", nr books: " << book_per_name_.size() << ", nr recipes: " << recipe_per_name_.size() << std::endl;
        }

    private:
        Uri uri_;
        std::string display_name_;
        ScriptFns script_fns_;
        BookPerName book_per_name_;
        RecipePerName recipe_per_name_;
    };

} } 

#endif

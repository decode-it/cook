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

        Book() {}
        Book(std::string name): name_(name) {}

        std::string uri_hr() const {return uri_.str();}

        const std::string &name() const {return name_;}
        std::string display_name() const
        {
            if (display_name_.empty())
                return name();
            return display_name_;
        }

        bool set(const std::string &key, const std::string &value)
        {
            MSS_BEGIN(bool);
            if (false) {}
            else if (key == "display_name") {display_name_ = value;}
            else if (key == "script_filename") {script_fns_.push_back(value);}
            MSS_END();
        }

        const ScriptFns &script_filenames() const {return script_fns_;}

        Book &goc_book(const std::string &name)
        {
            auto &book = book_per_name_[name];
            book.name_ = name;
            return book;
        }

        Recipe *create_recipe(const std::string &name)
        {
            if (recipe_per_name_.count(name) > 0)
                return nullptr;
            auto &recipe = recipe_per_name_[name];
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
            os << "name: " << name_ << ", nr books: " << book_per_name_.size() << ", nr recipes: " << recipe_per_name_.size() << std::endl;
        }

    private:
        Uri uri_;
        std::string name_;
        std::string display_name_;
        ScriptFns script_fns_;
        BookPerName book_per_name_;
        RecipePerName recipe_per_name_;
    };

} } 

#endif

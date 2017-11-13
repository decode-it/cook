#ifndef HEADER_cook_view_Proxy_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_Proxy_hpp_ALREADY_INCLUDED

#include "cook/model/Uri.hpp"
#include "cook/presenter/Interface.hpp"
#include <string>
#include <functional>
#include <iostream>
#include <memory>

namespace cook { namespace view { namespace proxy { 

    class Recipe
    {
    public:
        Recipe(const presenter::Reference &presenter): presenter_(presenter) { }
        Recipe(const presenter::Reference &presenter, const model::Uri &uri): presenter_(presenter), uri_(uri) { }
        Recipe(const Recipe &rhs): presenter_(rhs.presenter_), uri_(rhs.uri_) { }

        std::string uri_hr() const {return uri_.str('/', '/');}

    private:
        presenter::Reference presenter_;

        model::Uri uri_;
    };

    class Book
    {
    public:
        Book(const presenter::Reference &presenter): presenter_(presenter) { }
        Book(const presenter::Reference &presenter, const model::Uri &uri): presenter_(presenter), uri_(uri) { }
        Book(const Book &rhs): presenter_(rhs.presenter_), uri_(rhs.uri_) { }

        std::string uri_hr() const {return uri_.str('/', '/');}

        void print()
        {
            std::cout << this << " Book.print(): " << uri_hr() << std::endl;
        }
        void chai_book(const std::string &name, std::function<void(Book &)> callback)
        {
            auto uri = uri_;
            uri.add_path_part(name);
            Book book{presenter_, uri};
            presenter_.set("model.book.create", book.uri_hr());
            callback(book);
        }
        void chai_recipe_3(const std::string &name, const std::string &type, std::function<void(Recipe &)> callback)
        {
            auto uri = uri_;
            uri.set_name(name);
            Recipe recipe{presenter_, uri};
            Strings strings = {recipe.uri_hr(), type, working_directory().string()};
            if (!presenter_.set("model.recipe.create", strings))
            {
                std::ostringstream oss; oss << "Recipe \"" << strings[0] << "\" already exists";
                throw chaiscript::exception::eval_error(oss.str());
            }
#if 0
            if (!presenter_.set("model.recipe.type", type))
            {
                std::ostringstream oss; oss << "Unsupported recipe type \"" << type << "\" for \"" << name << "\"";
                throw chaiscript::exception::eval_error(oss.str());
            }
            if (!presenter_.set("model.recipe.working_directory", working_directory().string()))
            {
                std::ostringstream oss; oss << "Unsupported recipe type \"" << type << "\" for \"" << name << "\"";
                throw chaiscript::exception::eval_error(oss.str());
            }
#endif
            callback(recipe);
        }
        void chai_recipe_2(const std::string &name, std::function<void()> callback) { chai_recipe_3(name, "", callback); }
    private:
        presenter::Reference presenter_;

        model::Uri uri_;
    };

} } } 

#endif

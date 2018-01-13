#ifndef HEADER_cook_view_chai_Book_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_chai_Book_hpp_ALREADY_INCLUDED

#include "cook/view/chai/RunnerInfo.hpp"
#include "cook/view/chai/Engine.hpp"
#include "cook/view/chai/Recipe.hpp"
#include "cook/view/Logger.hpp"
#include "cook/presenter/Interface.hpp"
#include "cook/model/Uri.hpp"
#include "gubg/stream.hpp"
#include <functional>
#include <iostream>

namespace cook { namespace view { namespace chai { 

    class Book
    {
    public:
        Book(RunnerInfo &info): info_(info) {}
        Book(RunnerInfo &info, model::Uri uri): info_(info), uri_(uri) {}

        void chai_print() const
        {
            std::cout << "Book " << uri_.str('/','/') << std::endl;
        }
        void chai_display_name(const std::string &dn)
        {
            info_.logger.log(Info) << info_.indent() << ">> Setting display name to " << dn << std::endl;
            const Strings args = {uri_.str(), dn};
            info_.presenter.set("model.book.display_name", args);
            info_.logger.log(Info) << info_.indent() << "<< Setting display name to " << dn << std::endl;
        }
        void chai_book(const std::string &name, std::function<void(Book &)> callback)
        {
            info_.logger.log(Info) << info_.indent() << ">> Book " << name << std::endl;
            model::Uri uri = uri_;
            uri.add_path_part(name);
            info_.presenter.set("model.book.create", uri.str());
            Book book{info_, uri};
            callback(book);
            info_.logger.log(Info) << info_.indent() << "<< Book " << name << std::endl;
        }
        void chai_recipe_3(const std::string &name, const std::string &type, std::function<void(Recipe &)> callback)
        {
            info_.logger.log(Info) << info_.indent() << ">> Recipe " << name << " for type \"" << type << "\"" << std::endl;
            model::Uri uri = uri_;
            uri.set_name(name);
            const Strings args = {uri.str(), type, info_.working_directory().string()};
            if (!info_.presenter.set("model.recipe.create", args))
            {
                const std::string & error_msg = gubg::stream([&](auto & oss) { oss << "Recipe \"" << uri << "\" already exists"; });
                throw chaiscript::exception::eval_error(error_msg);
            }
            Recipe recipe{info_, uri};
            callback(recipe);
            info_.logger.log(Info) << info_.indent() << "<< Recipe " << name << " for type \"" << type << "\"" << std::endl;
        }
        void chai_recipe_2(const std::string &name, std::function<void(Recipe &)> callback) { chai_recipe_3(name, "", callback); }

    private:
        RunnerInfo &info_;
        model::Uri uri_;
    };

} } } 

#endif

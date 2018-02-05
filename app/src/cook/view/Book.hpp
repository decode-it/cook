#ifndef HEADER_cook_view_Book_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_Book_hpp_ALREADY_INCLUDED

#include "cook/api/Book.h"
#include "cook/view/RunnerInfo.hpp"
#include "cook/view/Recipe.hpp"
#include "cook/view/Logger.hpp"
#include "cook/model/Uri.hpp"
#include "gubg/stream.hpp"
#include <functional>
#include <iostream>

namespace cook { namespace view {

    class Book
    {
    public:
        Book(RunnerInfo &info, cook_api_Element_t book): info_(info), book_(book) {}

        void print() const
        {
            std::cout << "Book " << cook_api_Book_get_display_name(book_) << std::endl;
        }
        void display_name(const std::string &dn)
        {
            auto l = info_.log_object(cook_NotificationType_Info, [&](auto & str) { str << "Setting display name to " << dn; });
            cook_api_Book_set_display_name(book_, dn.c_str());
        }
        void book(const std::string &name, std::function<void(Book &)> callback)
        {
            auto l = info_.log_object(cook_NotificationType_Info, [&](auto & str) {str << "Book " << name; });
            cook_api_Element_t child = cook_api_Book_goc_subbook(book_, name.c_str());

            if (!cook_api_Element_valid(child))
            {
                const std::string & error_msg = gubg::stream([&](auto & oss) { oss << "Unable to get subbook \"" << name << "\""; });
                info_.notify_error(error_msg);
                return;
            }

            Book book {info_, child};
            callback(book);
        }
        void recipe_3(const std::string &name, const std::string &type, std::function<void(Recipe &)> callback)
        {
            auto l = info_.log_object(cook_NotificationType_Info, [&](auto & str) { str << "Recipe " << name << " for type \"" << type << "\""; });

            cook_api_Element_t child = cook_api_Book_create_recipe(book_, name.c_str(), type.c_str(), info_.working_directory().c_str());
            if (!cook_api_Element_valid(child))
            {
                const std::string & error_msg = gubg::stream([&](auto & oss) { oss << "Unable to create child \"" << name << "\""; });
                info_.notify_error(error_msg);
                return;
            }

            Recipe recipe{info_, child};
            callback(recipe);
        }
        void recipe_2(const std::string &name, std::function<void(Recipe &)> callback) { recipe_3(name, "", callback); }

    private:
        cook_api_Element_t book_;
        RunnerInfo &info_;
    };

} }

#endif

#ifndef HEADER_cook_view_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_Recipe_hpp_ALREADY_INCLUDED

#include "cook/api/Recipe.h"
#include "cook/view/File.hpp"
#include "cook/view/RunnerInfo.hpp"
#include "cook/view/Logger.hpp"
#include "cook/model/Uri.hpp"
#include "cook/File.hpp"
#include "gubg/stream.hpp"
#include <vector>
#include <functional>

namespace cook { namespace view {

class Recipe
{
public:
    using FilterPredicate = std::function<bool (File)>;

    Recipe(RunnerInfo &info, cook_api_Element_t recipe): recipe_(recipe), info_(info) {}

    static cook_bool_t filter_callback(void * context, cook_Handle_t file)
    {
        const FilterPredicate & pred = *static_cast<const FilterPredicate *>(context);
        if (!pred)
            return true;
        else
           return pred(File(file));
    }

    void print() const
    {
        std::cout << "Recipe " << cook_api_Recipe_get_display_name(recipe_) << std::endl;
    }
    void add_select_4(const std::string & dir, const std::string & pattern, cook_Flags_t flags, FilterPredicate functor)
    {       
        auto l = info_.log_object(cook_NotificationType_Info, [&](auto & str) { str << "Add files from " << dir << " // " << pattern << " with default type " << flags; });
        cook_api_Recipe_add(recipe_, dir.c_str(), pattern.c_str(), flags, &Recipe::filter_callback, &functor);
    }
    void add_select_3(const std::string &dir, const std::string &pattern, FilterPredicate functor)  { add_select_4(dir, pattern, {}, functor); }
    void add_select_2(const std::string &dir, FilterPredicate functor)                              { add_select_4(dir, "**", {}, functor); }
    void add_select_1(FilterPredicate functor)                                                      { add_select_4("",  "**", {}, functor); }
    void add_3(const std::string &dir, const std::string &pattern, cook_Flags_t flags)              { add_select_4(dir, pattern, flags,    FilterPredicate()); }
    void add_2(const std::string &dir, const std::string &pattern)                                  { add_select_4(dir, pattern, {}, FilterPredicate()); }
    void add_1(const std::string &pattern)                                                          { add_select_4("",  pattern, {}, FilterPredicate()); }
    void depends_on(const std::string &rn)
    {
        auto l = info_.log_object(cook_NotificationType_Info, [&](auto & str) { str << "Adding dependency on " << rn; });
        cook_api_Recipe_add_dependency(recipe_, rn.c_str());
    }
    void display_name(const std::string &dn)
    {
        auto l = info_.log_object(cook_NotificationType_Info, [&](auto & str) { str << "Setting display name to " << dn; });
        cook_api_Recipe_set_display_name(recipe_, dn.c_str());
    }
    void define_K_1(const std::string & key)                                                                    { define_KV_3(key,  "", cook_Flags_Overwrite_Never); }
    void define_KV_2(const std::string & key, const std::string & value)                                        { define_KV_3(key,  value, cook_Flags_Overwrite_Never); }
    void define_K_2(const std::string & key, cook_Flags_t flags)
    {
        auto l = info_.log_object(cook_NotificationType_Info, [&](auto & str) { str << "Adding define " << key << " ("<< flags << ")"; });
        cook_api_Recipe_add_define(recipe_, key.c_str(), nullptr, flags);
    }
    void define_KV_3(const std::string & key, const std::string & value, cook_Flags_t flags)
    {
        auto l = info_.log_object(cook_NotificationType_Info, [&](auto & str) { str << "Adding define " << key << " with value " << value << " (" << flags << ")"; });
        cook_api_Recipe_add_define(recipe_, key.c_str(), value.c_str(), flags);
    }
    void library_2(const std::string &name, cook_Flags_t propagation)
    {
        auto l = info_.log_object(cook_NotificationType_Info, [&](auto & str) { str << "Adding library " << name; });
        cook_api_Recipe_add_library(recipe_, name.c_str(), propagation);
    }

    void library_1(const std::string &name)
    {
        library_2(name, {});
    }

    void library_path(const std::string & name)
    {
        auto l = info_.log_object(cook_NotificationType_Info, [&](auto & str) { str << "Adding library path " << name; });
        cook_api_Recipe_add_library_path(recipe_, name.c_str(), {});
    }
    void include_path_1(const std::string & name) { include_path_2(name, cook_Flags_Propagation_Private); }

    void include_path_2(const std::string & name, cook_Flags_t propagation)
    {
        auto l = info_.log_object(cook_NotificationType_Info, [&](auto & str) { str << "Adding include path " << name << " (" << propagation << ")"; });
        cook_api_Recipe_add_include_path(recipe_, name.c_str(), propagation);
    }

private:
    cook_api_Element_t recipe_;
    RunnerInfo &info_;
};

} }

#endif

#ifndef HEADER_cook_view_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_Recipe_hpp_ALREADY_INCLUDED

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
    using C = presenter::Command;

    Recipe(RunnerInfo &info): info_(info) {}
    Recipe(RunnerInfo &info, model::Uri uri): info_(info), uri_(uri) {}

    void print() const
    {
        std::cout << "Recipe " << uri_.str('/','/') << std::endl;
    }
    void add_select_4(const std::string & dir, const std::string & pattern, FileType option, const FileFilterPredicate & functor)
    {       
        auto l = info_.log_object(Info, [&](auto & str) { str << "Add files from " << dir << " // " << pattern << " with default type " << option; });

        const auto & args = as_any(uri_.str(), dir, pattern, option, functor);
        info_.presenter->set({C::model, C::recipe, C::add}, args);
    }
    void add_select_3(const std::string &dir, const std::string &pattern, const FileFilterPredicate & functor)  { add_select_4(dir, pattern, Unknown, functor); }
    void add_select_2(const std::string &dir, const FileFilterPredicate & functor)                              { add_select_4(dir, "**", Unknown, functor); }
    void add_select_1(const FileFilterPredicate & functor)                                                      { add_select_4("",  "**", Unknown, functor); }
    void add_3(const std::string &dir, const std::string &pattern, FileType type)                               { add_select_4(dir, pattern, type,    FileFilterPredicate()); }
    void add_2(const std::string &dir, const std::string &pattern)                                              { add_select_4(dir, pattern, Unknown, FileFilterPredicate()); }
    void add_1(const std::string &pattern)                                                                      { add_select_4("",  pattern, Unknown, FileFilterPredicate()); }
    void depends_on(const std::string &rn)
    {
        auto l = info_.log_object(Info, [&](auto & str) { str << "Adding dependency on " << rn; });

        const auto & args = as_any(uri_.str(), rn);
        info_.presenter->set({C::model, C::recipe, C::depends_on}, args);
    }
    void display_name(const std::string &dn)
    {
        auto l = info_.log_object(Info, [&](auto & str) { str << "Setting display name to " << dn; });

        const auto & args = as_any(uri_.str(), dn);
        info_.presenter->set({C::model, C::recipe, C::display_name}, args);
    }
    void define_K_1(const std::string & key)                                                                    { define_KV_3(key,  "", Overwrite::Never); }
    void define_KV_2(const std::string & key, const std::string & value)                                        { define_KV_3(key,  value, Overwrite::Never); }
    void define_K_2(const std::string & key, Overwrite overwrite)
    {
        auto l = info_.log_object(Info, [&](auto & str) { str << "Adding define " << key << " (overwrite=" << overwrite << ")"; });

        const auto & args = as_any(uri_.str(), overwrite, key);
        info_.presenter->set({C::model, C::recipe, C::define}, args);
    }
    void define_KV_3(const std::string & key, const std::string & value, Overwrite overwrite)
    {
        auto l = info_.log_object(Info, [&](auto & str) { str << "Adding define " << key << " with value " << value << " (overwrite=" << overwrite << ")"; });

        const auto & args = as_any(uri_.str(), overwrite, key, value);
        info_.presenter->set({C::model, C::recipe, C::define}, args);
    }
    void library(const std::string &name)
    {
        auto l = info_.log_object(Info, [&](auto & str) { str << "Adding library " << name; });

        const auto & args = as_any(uri_.str(), name);
        info_.presenter->set({C::model, C::recipe, C::library}, args);
    }

    void library_path(const std::string & name)
    {
        auto l = info_.log_object(Info, [&](auto & str) { str << "Adding library path " << name; });

        const auto & args = as_any(uri_.str(), name);
        info_.presenter->set({C::model, C::recipe, C::library_path}, args);
    }
    void include_path_1(const std::string & name)
    {
        include_path_2(name, Propagation::Local);
    }
    void include_path_2(const std::string & name, Propagation propagation)
    {
        auto l = info_.log_object(Info, [&](auto & str) { str << "Adding include path " << name << " (" << propagation << ")"; });

        const auto & args = as_any(uri_.str(), name, propagation);
        info_.presenter->set({C::model, C::recipe, C::include_path}, args);
    }

private:
    RunnerInfo &info_;
    model::Uri uri_;
};

} }

#endif

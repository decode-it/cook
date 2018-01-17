#ifndef HEADER_cook_view_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_Recipe_hpp_ALREADY_INCLUDED

#include "cook/view/RunnerInfo.hpp"
#include "cook/view/Logger.hpp"
#include "cook/view/File.hpp"
#include "cook/presenter/Interface.hpp"
#include "cook/model/Uri.hpp"
#include "gubg/stream.hpp"
#include "gubg/file/System.hpp"
#include <vector>
#include <functional>

namespace cook { namespace view {

using AddFileFunctor = std::function<bool (File &)>;

enum class Overwrite { Never, IfSame, Always };
inline std::string hr(Overwrite value)
{
    switch(value)
    {
#define CASES(NAME, VALUE) case Overwrite::NAME: return VALUE
    CASES(Never, "never");
    CASES(IfSame, "if_same");
    CASES(Always, "always");
#undef CASES
    default:
        return "Unknown";
    }
}

class Recipe
{
public:
    Recipe(RunnerInfo &info): info_(info) {}
    Recipe(RunnerInfo &info, model::Uri uri): info_(info), uri_(uri) {}

    void print() const
    {
        std::cout << "Recipe " << uri_.str('/','/') << std::endl;
    }
    void add_select_4(const std::string & dir, const std::string & pattern, FileType option, const AddFileFunctor & functor)
    {
        auto l = info_.log_object(Info, [&](auto & str) { str << "Add files from " << dir << " // " << pattern << " as " << option << " with selection functor "; });

        // get the path
        std::filesystem::path pdir = dir.empty() ? "." : dir;
        if (pdir.is_relative())
        {
            const Strings args = { uri_.str() };
            std::string wd;
            if (!info_.presenter.get("model.recipe.working_directory", args, wd))
            {
                const std::string & error_msg = gubg::stream([&](auto & oss) { oss << "Could not request the working directory for recipe " << uri_; });
                info_.notify_error(error_msg);
            }
            pdir = wd / pdir;
        }

        auto add_file = [&](const std::filesystem::path & fp)
        {
            std::filesystem::path rel;
            {
                auto dir_it = pdir.begin();
                auto dir_end = pdir.end();
                for (const auto &part: fp)
                {
                    if (dir_it != dir_end && part == *dir_it)
                    {
                        ++dir_it;
                        continue;
                    }
                    rel /= part;
                }
            }

            File f(pdir, rel);
            f.type = option;

            if (!functor || functor(f))
            {
                const Strings args = { uri_.str(), dir, rel.string(), to_string(f.type) };
                if (!info_.presenter.set("model.recipe.add_file", args))
                {
                    const std::string & error_msg = gubg::stream([&](auto & oss) { oss << "Could not add file to recipe " << uri_; });
                    info_.notify_error(error_msg);
                }
            }
        };

        gubg::file::each_glob(pattern, add_file, pdir);

    }
    void add_select_3(const std::string &dir, const std::string &pattern, const AddFileFunctor & functor)   { add_select_4(dir, pattern, Unknown, functor); }
    void add_select_2(const std::string &dir, const AddFileFunctor & functor)                               { add_select_4(dir, "**", Unknown, functor); }
    void add_select_1(const AddFileFunctor & functor)                                                       { add_select_4("",  "**", Unknown, functor); }
    void add_3(const std::string &dir, const std::string &pattern, FileType type)                           { add_select_4(dir, pattern, type,    AddFileFunctor()); }
    void add_2(const std::string &dir, const std::string &pattern)                                          { add_select_4(dir, pattern, Unknown, AddFileFunctor()); }
    void add_1(const std::string &pattern)                                                                  { add_select_4("",  pattern, Unknown, AddFileFunctor()); }
    void depends_on(const std::string &rn)
    {
        auto l = info_.log_object(Info, [&](auto & str) { str << "Adding dependency on " << rn; });

        const Strings args = {uri_.str(), rn};
        info_.presenter.set("model.recipe.depends_on", args);
    }
    void display_name(const std::string &dn)
    {
        auto l = info_.log_object(Info, [&](auto & str) { str << "Setting display name to " << dn; });

        const Strings args = {uri_.str(), dn};
        info_.presenter.set("model.recipe.display_name", args);
    }
    void define_K_1(const std::string & key)
    {
        define_KV_3(key,  "", Overwrite::Never);
    }
    void define_KV_2(const std::string & key, const std::string & value)
    {
        define_KV_3(key,  value, Overwrite::Never);
    }
    void define_K_2(const std::string & key, Overwrite overwrite)
    {
        define_KV_3(key,  "", overwrite);
    }
    void define_KV_3(const std::string & key, const std::string & value, Overwrite overwrite)
    {
        auto l = info_.log_object(Info, [&](auto & str) { str << "Adding define " << key << " with value " << value << " (overwrite=" << hr(overwrite) << ")"; });

        const Strings args = {uri_.str(), key, value, hr(overwrite) };
        info_.presenter.set("model.recipe.define", args);
    }
    void library(const std::string &name)
    {
        auto l = info_.log_object(Info, [&](auto & str) { str << "Adding library " << name; });

        const Strings args = {uri_.str(), name};
        info_.presenter.set("model.recipe.library", args);
    }

    void library_path(const std::string & name)
    {
        auto l = info_.log_object(Info, [&](auto & str) { str << "Adding library path " << name; });

        const Strings args = {uri_.str(), name};
        info_.presenter.set("model.recipe.library_path", args);
    }

private:
    RunnerInfo &info_;
    model::Uri uri_;
};

} }

#endif

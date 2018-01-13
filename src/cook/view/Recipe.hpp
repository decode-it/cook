#ifndef HEADER_cook_view_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_Recipe_hpp_ALREADY_INCLUDED

#include "cook/view/RunnerInfo.hpp"
#include "cook/view/Logger.hpp"
#include "cook/view/File.hpp"
#include "cook/presenter/Interface.hpp"
#include "cook/model/Uri.hpp"
#include "gubg/stream.hpp"
#include <vector>
#include <functional>

namespace cook { namespace view {

    class Recipe
    {
    public:
        Recipe(RunnerInfo &info): info_(info) {}
        Recipe(RunnerInfo &info, model::Uri uri): info_(info), uri_(uri) {}

        void print() const
        {
            std::cout << "Recipe " << uri_.str('/','/') << std::endl;
        }
        void add_4(const std::string & dir, const std::string & pattern, const std::string & option, const std::function<bool (File &)> & functor)
        {
        }
        void add_3(const std::string &dir, const std::string &pattern, const std::string &option)
        {
            auto l = info_.log_object(Info, [&](auto & str) { str << "Add files from " << dir << " // " << pattern << " as " << option; });

            const Strings args = {uri_.str(), dir, pattern, option};
            if (!info_.presenter.set("model.recipe.add", args))
            {
                const std::string & error_msg = gubg::stream([&](auto & oss) { oss << "Could not add files to recipe " << uri_; });
                info_.notify_error(error_msg);
            }
        }
        void add_2(const std::string &dir, const std::string &pattern) { add_3(dir, pattern, ""); }
        void add_1(const std::string &pattern) { add_3("", pattern, ""); }
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

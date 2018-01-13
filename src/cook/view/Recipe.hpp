#ifndef HEADER_cook_view_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_Recipe_hpp_ALREADY_INCLUDED

#include "cook/view/RunnerInfo.hpp"
#include "cook/view/Logger.hpp"
#include "cook/presenter/Interface.hpp"
#include "cook/model/Uri.hpp"
#include "gubg/stream.hpp"
#include <vector>
#include <functional>
#include <iostream>

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
        void add_3(const std::string &dir, const std::string &pattern, const std::string &option)
        {
            info_.logger.log(Info) << info_.indent() << ">> Add files from " << dir << " // " << pattern << " as " << option << std::endl;
            const Strings args = {uri_.str(), dir, pattern, option};
            if (!info_.presenter.set("model.recipe.add", args))
            {
                const std::string & error_msg = gubg::stream([&](auto & oss) { oss << "Could not add files to recipe " << uri_; });
                info_.notify_error(error_msg);
            }
            info_.logger.log(Info) << info_.indent() << "<< Add files from " << dir << " // " << pattern << " as " << option << std::endl;
        }
        void add_2(const std::string &dir, const std::string &pattern) { add_3(dir, pattern, ""); }
        void add_1(const std::string &pattern) { add_3("", pattern, ""); }
        void depends_on(const std::string &rn)
        {
            info_.logger.log(Info) << info_.indent() << ">> Adding dependency on " << rn << std::endl;
            const Strings args = {uri_.str(), rn};
            info_.presenter.set("model.recipe.depends_on", args);
            info_.logger.log(Info) << info_.indent() << "<< Adding dependency on " << rn << std::endl;
        }
        void display_name(const std::string &dn)
        {
            info_.logger.log(Info) << info_.indent() << ">> Setting display name to " << dn << std::endl;
            const Strings args = {uri_.str(), dn};
            info_.presenter.set("model.recipe.display_name", args);
            info_.logger.log(Info) << info_.indent() << "<< Setting display name to " << dn << std::endl;
        }
        void library(const std::string &name)
        {
            info_.logger.log(Info) << info_.indent() << ">> Adding library " << name << std::endl;
            const Strings args = {uri_.str(), name};
            info_.presenter.set("model.recipe.library", args);
            info_.logger.log(Info) << info_.indent() << "<< Adding library " << name << std::endl;
        }

        void library_path(const std::string & name)
        {
            info_.logger.log(Info) << info_.indent() << ">> Adding library path " << name << std::endl;
            const Strings args = {uri_.str(), name};
            info_.presenter.set("model.recipe.library_path", args);
            info_.logger.log(Info) << info_.indent() << "<< Adding library path " << name << std::endl;
        }

    private:
        RunnerInfo &info_;
        model::Uri uri_;
    };

} }

#endif

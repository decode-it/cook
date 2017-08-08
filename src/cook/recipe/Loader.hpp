#ifndef HEADER_cook_recipe_Loader_hpp_ALREADY_INCLUDED
#define HEADER_cook_recipe_Loader_hpp_ALREADY_INCLUDED

#include "cook/Options.hpp"
#include "cook/recipe/Description.hpp"
#include "cook/Codes.hpp"
#include "cook/chai/Engine.hpp"
#include "gubg/mss.hpp"
#include <map>
#include <iostream>

namespace cook { namespace recipe { 

    using DescriptionPerAlias = std::map<Alias, Description>;

    class Loader
    {
        private:
            static constexpr const char *logns = "Loader";

        public:
            Loader(const Options &options);

            ReturnCode load(const std::filesystem::path &dir, const std::filesystem::path &fn)
            {
                MSS_BEGIN(ReturnCode, logns);
                L("Loading recipes from " << (dir/fn));
                path_stack_.push_back(dir);
                MSS(chai_engine_.eval_file(dir/fn));
                path_stack_.pop_back();
                L("Loaded " << descriptions_.size() << " recipes");
                MSS_END();
            }

            ReturnCode resolve()
            {
                MSS_BEGIN(ReturnCode, logns);

                bool changed;
                do
                {
                    changed = false;
                    for (auto &p: descriptions_)
                    {
                        auto &a_alias = p.first;
                        auto &a = p.second;
                        std::set<std::string> new_deps;
                        {
                            const auto &deps = a.dependencies();
                            for (const auto &b_alias: deps)
                            {
                                auto b_it = descriptions_.find(b_alias);
                                MSS(b_it != descriptions_.end(), std::cerr << "Error: Could not find dependency " << b_alias << " needed by " << a_alias << std::endl);
                                if (b_alias != a_alias.str())
                                    for (const auto &c_alias: b_it->second.dependencies())
                                        if (c_alias != a_alias.str() && deps.count(c_alias) != 0)
                                            new_deps.insert(c_alias);
                            }
                        }
                        if (!new_deps.empty())
                            changed = true;
                        for (const auto &new_alias: new_deps)
                            a.depends_on(new_alias);
                    }
                } while (changed);

                for (auto &p: descriptions_)
                {
                    auto &a_alias = p.first;
                    auto &a = p.second;
                    const auto &deps = a.dependencies();
                    for (const auto &b_alias: deps)
                    {
                        auto b_it = descriptions_.find(b_alias);
                        MSS(b_it != descriptions_.end(), std::cerr << "Error: Could not find dependency " << b_alias << " needed by " << a_alias << std::endl);
                        a.merge(b_it->second);
                    }
                }

                MSS_END();
            }

            ReturnCode get(const Description *&description, const Alias &alias) const
            {
                MSS_BEGIN(ReturnCode);
                auto p = descriptions_.find(alias);
                MSS(p != descriptions_.end());
                description = &p->second;
                MSS_END();
            }

            //Methods exposed to chai
            bool create_new_recipe_3(const std::string &ns, const std::string &tag, std::function<void (Description &)> callback)
            {
                return create_new_recipe_(ns, tag, "", callback);
            }
            bool create_new_recipe_4(const std::string &ns, const std::string &tag, const std::string &extra, std::function<void (Description &)> callback)
            {
                return create_new_recipe_(ns, tag, extra, callback);
            }
            bool add_dir(const std::string &dir);
            bool log(const std::string &dir);

        private:
            bool create_new_recipe_(const std::string &ns, const std::string &tag, const std::string &extra, std::function<void (Description &)> callback);
            std::string indent_() const;

            const Options &options_;
            chai::Engine chai_engine_;
            DescriptionPerAlias descriptions_;
            std::vector<std::filesystem::path> path_stack_;
    };

} } 

#endif

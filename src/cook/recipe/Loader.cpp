#include "cook/recipe/Loader.hpp"
#include "cook/recipe/Description.hpp"
#include "gubg/debug.hpp"
#include <iostream>
#include <cassert>

namespace cook { namespace recipe { 

    static constexpr const char *logns = "Loader";

    ReturnCode Loader::load(const std::filesystem::path &dir, const std::filesystem::path &fn)
    {
        MSS_BEGIN(ReturnCode, logns);
        L("Loading recipes from " << (dir/fn));
        path_stack_.push_back(dir);
        MSS(chai_engine_.eval_file(dir/fn));
        path_stack_.pop_back();
        L("Loaded " << descriptions_.size() << " recipes");
        MSS_END();
    }

    ReturnCode Loader::resolve()
    {
        MSS_BEGIN(ReturnCode);

        bool changed;
        do
        {
            changed = false;
            L("***Starting again:");
            for (auto &p: descriptions_)
            {
                auto &a_alias = p.first;
                auto &a = p.second;
                L(C(a_alias));
                std::set<std::string> new_deps;
                {
                    const auto &deps = a.dependencies();
                    for (const auto &b_alias: deps)
                    {
                        L("  depends on " << C(b_alias));
                        auto b_it = descriptions_.find(b_alias);
                        MSS(b_it != descriptions_.end(), std::cerr << "Error: Could not find dependency " << b_alias << " needed by " << a_alias << std::endl);
                        if (b_alias != a_alias.str())
                            for (const auto &c_alias: b_it->second.dependencies())
                            {
                                L("    which depends on " << C(c_alias));
                                if (c_alias != a_alias.str() && deps.count(c_alias) == 0)
                                    new_deps.insert(c_alias);
                            }
                    }
                }
                if (!new_deps.empty())
                {
                    changed = true;
                    for (const auto &new_alias: new_deps)
                    {
                        L("  adding new dependency " << new_alias);
                        a.depends_on(new_alias);
                    }
                }
            }
        } while (changed);
        L("***No more dependencies to resolve");

        for (auto &p: descriptions_)
        {
            auto &a_alias = p.first;
            auto &a = p.second;
            L(C(a_alias));
            const auto &deps = a.dependencies();
            for (const auto &b_alias: deps)
            {
                L("  depends on " << C(b_alias));
                auto b_it = descriptions_.find(b_alias);
                MSS(b_it != descriptions_.end(), std::cerr << "Error: Could not find dependency " << b_alias << " needed by " << a_alias << std::endl);
                a.merge(b_it->second);
            }
        }

        MSS_END();
    }

    ReturnCode Loader::get(const Description *&description, const Alias &alias) const
    {
        MSS_BEGIN(ReturnCode);
        auto p = descriptions_.find(alias);
        MSS(p != descriptions_.end());
        description = &p->second;
        MSS_END();
    }

    void Loader::stream(std::ostream &os) const
    {
        for (auto &p: descriptions_)
        {
            auto &alias = p.first;
            auto &descr = p.second;
            os << "[recipe](name:" << alias << ")(ns:" << alias.ns() << ")(tag:" << alias.tag() << "){" << std::endl;
            {
                IncludePaths include_paths;
                descr.get_all_include_paths(include_paths);
                for (const auto &path: include_paths)
                    os << "  [include_path](path:" << path.native() << ")" << std::endl;
            }
            {
                auto to_s = [](file::Type type)
                {
                    switch (type)
                    {
                        case file::Source: return "source"; break;
                        case file::Header: return "header"; break;
                        case file::ForceInclude: return "force_include"; break;
                    }
                    return "";
                };
                auto stream_files = [&](const FileInfo &file_info)
                {
                    for (const auto &p: file_info)
                        os << "  [file](type:" << to_s(p.second.type) << ")(path:" << p.first.native() << ")" << std::endl;
                };
                stream_files(descr.sources());
                stream_files(descr.headers());
            }
            for (const auto &dep: descr.dependencies())
                os << "  [depends_on](name:" << dep << ")" << std::endl;
            os << "}" << std::endl;
        }
    }

    bool Loader::create_new_recipe_(const std::string &ns, const std::string &tag, const std::string &extra, std::function<void (Description &)> callback)
    {
        MSS_BEGIN(bool, "create_new_recipe");
        const Alias alias(ns, tag);
        assert(!path_stack_.empty());
        auto base_dir = path_stack_.back();
        L(C(alias)C(base_dir));
        auto it = descriptions_.find(alias);
        if (it != descriptions_.end())
        {
            if (false) {}
            else if (extra == "")
            {
                MSS(false, std::cout << "Recipe " << alias << " already exists in " << it->second.base() << std::endl);
            }
            else if (extra == "unless_exists")
            {
                if (options_.verbose >= 1)
                    std::cout << "Recipe for " << alias << " already exists in " << it->second.base() << ", skipping the one from " << base_dir << std::endl;
                MSS_RETURN_OK();
            }
            else if (extra == "overwrite_if_exists")
            {
                if (options_.verbose >= 1)
                    std::cout << "Overwriting previous recipe for " << alias << " (old was in " << it->second.base() << ", new is in " << base_dir << ")" << std::endl;
                descriptions_.erase(it);
            }
            else
            {
                MSS(false, std::cout << "Error: Unknown " << C(extra) << " parameter specified in " << base_dir << std::endl);
            }
        }
        auto p = descriptions_.emplace(alias, alias);
        assert(p.second);
        auto &description = p.first->second;
        description.set_base(base_dir);
        callback(description);
        MSS_END();
    }

    bool Loader::add_dir(const std::string &dir)
    {
        MSS_BEGIN(bool);
        if (options_.verbose >= 1)
            std::cout << indent_() << "Adding dir " << dir << std::endl;
        MSS(load(dir, "recipes.chai"));
        MSS_END();
    }

    std::string Loader::indent_() const
    {
        return std::string(2*(path_stack_.size()-1), ' ');
    }

    bool Loader::log(const std::string &str)
    {
        MSS_BEGIN(bool);
        if (options_.verbose >= 1)
            std::cout << indent_() << str << std::endl;
        MSS_END();
    }

    Loader::Loader(const Options &options): options_(options)
    {
        auto &chai = chai_engine_.raw();
        chai.add(chaiscript::fun(&Loader::create_new_recipe_3, this), "recipe");
        chai.add(chaiscript::fun(&Loader::create_new_recipe_4, this), "recipe");//Additional "extra" argument: "unless_exists" or "overwrite_if_exists"
        chai.add(chaiscript::fun(&Loader::add_dir, this), "add_dir");
        chai.add(chaiscript::fun(&Loader::log, this), "log");
        chai.add(chaiscript::fun(&Description::add_source), "add_source");
        chai.add(chaiscript::fun(&Description::add_header), "add_header");
        chai.add(chaiscript::fun(&Description::add_include_path), "add_include_path");
        chai.add(chaiscript::fun(&Description::add_library_path), "add_library_path");
        chai.add(chaiscript::fun(&Description::add_library), "add_library");
        chai.add(chaiscript::fun(&Description::depends_on), "depends_on");
        chai.add(chaiscript::fun(&Description::print), "print");
    }

} } 

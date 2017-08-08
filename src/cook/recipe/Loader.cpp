#include "cook/recipe/Loader.hpp"
#include "cook/recipe/Description.hpp"
#include "gubg/debug.hpp"
#include <iostream>
#include <cassert>

namespace cook { namespace recipe { 

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

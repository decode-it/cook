#include "cook/recipe/Loader.hpp"
#include "cook/recipe/Description.hpp"
#include "gubg/debug.hpp"
#include <iostream>

namespace cook { namespace recipe { 

    bool Loader::create_new_recipe(const std::string &ns, const std::string &tag, std::function<void (Description &)> callback)
    {
        MSS_BEGIN(bool);
        const Alias alias(ns, tag);
        auto p = descriptions_.emplace(alias, alias);
        MSS(p.second, std::cerr << "Recipe " << alias << " already exists" << std::endl);
        auto &description = p.first->second;
        callback(description);
        MSS_END();
    }

    Loader::Loader()
    {
        auto &chai = chai_engine_.raw();
        chai.add(chaiscript::fun(&Loader::create_new_recipe, this), "recipe");
        chai.add(chaiscript::fun(&Description::add_source), "add_source");
        chai.add(chaiscript::fun(&Description::add_header), "add_header");
        chai.add(chaiscript::fun(&Description::add_include_path), "add_include_path");
        chai.add(chaiscript::fun(&Description::add_library_path), "add_library_path");
        chai.add(chaiscript::fun(&Description::add_library), "add_library");
        chai.add(chaiscript::fun(&Description::depends_on), "depends_on");
        chai.add(chaiscript::fun(&Description::print), "print");
    }

} } 

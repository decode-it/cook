#include "cook/recipe/Loader.hpp"
#include "cook/recipe/Description.hpp"
#include "gubg/debug.hpp"
#include <iostream>

namespace cook { namespace recipe { 

    void create_new_recipe(const std::string &ns, const std::string &tag, std::function<void (Description &)> callback)
    {
        Description description(Alias(ns, tag));
        callback(description);
    }

    Loader::Loader()
    {
        auto &chai = chai_engine_.raw();
        chai.add(chaiscript::fun(create_new_recipe), "recipe");
        chai.add(chaiscript::fun(&Description::add_source), "add_source");
        chai.add(chaiscript::fun(&Description::add_header), "add_header");
        chai.add(chaiscript::fun(&Description::print), "print");
    }

} } 

#include "cook/Context.hpp"
#include "cook/algo/Book.hpp"
#include "cook/generator/graphviz/Dependency.hpp"
#include "cook/generator/graphviz/Component.hpp"
#include "cook/generator/CMake.hpp"
#include "cook/generator/Naft.hpp"
#include "cook/generator/Ninja.hpp"
#include "cook/generator/HTML.hpp"
#include "cook/process/toolchain/Manager.hpp"
#include "gubg/mss.hpp"
#include "gubg/Strange.hpp"
#include <cassert>
#include <algorithm>
#include <cctype>

namespace  { 
    /* const char *logns = ""; */
    const char *logns = nullptr;

    std::string to_lower(const std::string & str)
    {
        std::string res = str;
        std::for_each(res.begin(), res.end(), [](char & c) { c = std::tolower(c); });
        return res;
    }
} 

namespace cook {

Result Context::initialize()
{
    MSS_BEGIN(Result);

    MSS(toolchain_().initialize());

    // add the generators
    MSS(register_generator(std::make_shared<generator::graphviz::Dependency>()));
    MSS(register_generator(std::make_shared<generator::graphviz::Component>()));
    MSS(register_generator(std::make_shared<generator::CMake>()));
    MSS(register_generator(std::make_shared<generator::Naft>()));
    MSS(register_generator(std::make_shared<generator::Ninja>()));
    MSS(register_generator(std::make_shared<generator::HTML>()));

    MSS_END();
}

Result Context::initialize_menu(const std::list<model::Recipe*> & root_recipes)
{
    MSS_BEGIN(Result);

    MSS(lib_.resolve());
    MSS(menu_.construct(gubg::make_range(root_recipes)));

    MSS_END();
}

Result Context::register_generator(GeneratorPtr generator)
{
    MSS_BEGIN(Result, logns);

    MSS(!!generator);
    const std::string & name = generator->name();
    const std::string & lname = to_lower(name);

    MSG_MSS(!name.empty(), Error, "Cannot add a generator with an empty name");
    MSG_MSS(generators_.find(lname) == generators_.end(), Error, "A generator with name '" << name << "' already exists");

    generators_.emplace(lname, generator);

    MSS_END();
}

Context::GeneratorPtr Context::get_generator(const std::string & name) const
{
    const std::string & lname = to_lower(name);
    auto it = generators_.find(lname);
    return it == generators_.end() ? GeneratorPtr() : it->second;
}


model::Book * Context::root_book() const
{
    return lib_.root();
}

Result Context::find_recipe(model::Recipe *& recipe, const std::string & name) const
{
    MSS_BEGIN(Result, logns);

    // create the uri
    model::Uri uri(name);
    MSG_MSS(!uri.path().empty(), Error, "The supplied uri is empty");

    // forgive the fact that recipe is not absolute
    uri.set_absolute(true);

    // find the recipe in the root book
    MSG_MSS(lib_.find_recipe(recipe, uri), InternalError, "Error while finding uri '" << uri << "'");
 
    // make sure the recipe is found
    MSG_MSS(!!recipe, Error, "No recipe with name '" << uri << "' exists ");

    MSS_END();

}

OS Context::os() const
{
    return get_os();
}

void Context::add_toolchain_config(const std::string & key, const std::string & value)
{
    toolchain_().add_config(key, value);
}

void Context::add_toolchain_config(const std::string & key)
{
    toolchain_().add_config(key);
}

process::toolchain::Manager &Context::toolchain_() const
{
    if (!toolchain_ptr_)
        toolchain_ptr_.reset(new process::toolchain::Manager);
    return *toolchain_ptr_;
}
const process::toolchain::Manager &Context::toolchain() const
{
    return toolchain_();
}

process::toolchain::Manager & Context::toolchain()
{
    return toolchain_();
}

}

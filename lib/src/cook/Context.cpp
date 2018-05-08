#include "cook/Context.hpp"
#include "cook/algo/Book.hpp"
#include "cook/generator/graphviz/Dependency.hpp"
#include "cook/generator/graphviz/Component.hpp"
#include "cook/generator/qt/Generic.hpp"
#include "cook/generator/qt/Cook.hpp"
#include "cook/generator/CMake.hpp"
#include "cook/generator/Naft.hpp"
#include "cook/generator/Ninja.hpp"
#include "cook/process/toolchain/Manager.hpp"
#include "gubg/mss.hpp"
#include "gubg/Strange.hpp"
#include <cassert>

namespace  { 
    /* const char *logns = ""; */
    const char *logns = nullptr;
} 

namespace cook {

Result Context::initialize()
{
    MSS_BEGIN(Result);

    MSS(toolchain_().initialize());

    // add the generators
    MSS(register_generator(std::make_shared<generator::graphviz::Dependency>()));
    MSS(register_generator(std::make_shared<generator::graphviz::Component>()));
    MSS(register_generator(std::make_shared<generator::qt::Generic>()));
    MSS(register_generator(std::make_shared<generator::qt::Cook>()));
    MSS(register_generator(std::make_shared<generator::CMake>()));
    MSS(register_generator(std::make_shared<generator::Naft>()));
    MSS(register_generator(std::make_shared<generator::Ninja>()));

    // specify as project name the current directory
    {
        std::filesystem::path fn = gubg::filesystem::combine(std::filesystem::current_path(), dirs().recipe());
        if (!fn.empty())
        {
            auto it = --fn.end();
            set_project_name(it->string());
        }
    }

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

    MSG_MSS(!name.empty(), Error, "Cannot add a generator with an empty name");
    MSG_MSS(generators_.find(name) == generators_.end(), Error, "A generator with name '" << name << "' already exists");

    generators_.emplace(name, generator);

    MSS_END();
}

Context::GeneratorPtr Context::get_generator(const std::string & name) const
{
    auto it = generators_.find(name);
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

Result Context::set_toolchain(const std::string &toolchain)
{
    MSS_BEGIN(Result);
    gubg::Strange strange(toolchain);

    auto get_part = [&](std::string & part) {
        return strange.pop_until(part, '-') || strange.pop_all(part);
    };

    {
        std::string brand;
        get_part(brand);
        MSS(toolchain_().temp_set_brand(brand));
    }

    {
        std::string part;
        while(get_part(part))
            toolchain_().configure(part);
    }

    MSS_END();
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

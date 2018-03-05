#include "cook/Context.hpp"
#include "cook/algo/Book.hpp"
#include "cook/generator/graphviz/Dependency.hpp"
#include "cook/generator/graphviz/Component.hpp"
#include "cook/generator/CMake.hpp"
#include "cook/generator/Naft.hpp"
#include "gubg/mss.hpp"
#include <cassert>

namespace  { 
    /* const char *logns = ""; */
    const char *logns = nullptr;
} 

namespace cook {

bool Context::initialize()
{
    MSS_BEGIN(bool);

    // add the generator
    MSS(register_generator(std::make_shared<generator::graphviz::Dependency>()));
    MSS(register_generator(std::make_shared<generator::graphviz::Component>()));
    MSS(register_generator(std::make_shared<generator::CMake>()));
    MSS(register_generator(std::make_shared<generator::Naft>()));

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
    std::pair<model::Uri, bool> p = model::Uri::recipe_uri(name);

    MSG_MSS(p.second, Error, "Invalid recipe uri '" << name << "'");

    // forgive the fact that recipe is not absolute
    p.first.set_absolute(true);

    // find the recipe in the root book
    MSG_MSS(lib_.find_recipe(recipe, p.first), InternalError, "Error while finding uri '" << p.first << "'");

    // make sure the recipe is found
    MSG_MSS(!!recipe, Error, "No recipe with name '" << p.first << "' exists ");

    MSS_END();

}

}



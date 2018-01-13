#include "cook/view/chai/Recipe.hpp"
#include "cook/view/Recipe.hpp"
#include "chaiscript/chaiscript.hpp"

namespace cook { namespace view { namespace chai {

ModulePtr recipe_module()
{
    ModulePtr m(new chaiscript::Module());

    m->add(chaiscript::user_type<Recipe>(), "Recipe");
    m->add(chaiscript::constructor<Recipe(const Recipe &)>(), "Recipe");
    m->add(chaiscript::fun(&Recipe::print), "print");
    m->add(chaiscript::fun(&Recipe::add_select_3), "add");
    m->add(chaiscript::fun(&Recipe::add_select_2), "add");
    m->add(chaiscript::fun(&Recipe::add_select_1), "add");
    m->add(chaiscript::fun(&Recipe::add_3), "add");
    m->add(chaiscript::fun(&Recipe::add_2), "add");
    m->add(chaiscript::fun(&Recipe::add_1), "add");
    m->add(chaiscript::fun(&Recipe::depends_on), "depends_on");
    m->add(chaiscript::fun(&Recipe::display_name), "display_name");
    m->add(chaiscript::fun(&Recipe::library), "library");
    m->add(chaiscript::fun(&Recipe::library_path), "library_path");

    return m;
}

} } }

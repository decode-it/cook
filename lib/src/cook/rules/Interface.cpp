#include "cook/rules/Interface.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace rules {

void Interface::remove_file(model::Recipe & recipe, const LanguageTypePair & key, const ingredient::File & file) const
{
    recipe.erase(key, file);
}

} }

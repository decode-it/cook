#include "cook/rules/Interface.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace rules {

void Interface::remove_file(model::Recipe & recipe, const LanguageTypePair & key, const ingredient::File & file) const
{
    auto it = recipe.files().find(key);
    if (it == recipe.files().end())
        return;

    ingredient::Collection<ingredient::File> & files = it->second;

    auto it2 = files.find(file.key());
    if (it2 != files.end())
        files.erase(it2);
}

} }

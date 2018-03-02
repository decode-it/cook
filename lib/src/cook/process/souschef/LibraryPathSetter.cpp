#include "cook/process/souschef/LibraryPathSetter.hpp"
#include "cook/process/souschef/PathExtraction.hpp"
#include <set>

namespace cook { namespace process { namespace souschef {

Result LibraryPathSetter::process(model::Recipe & recipe, RecipeFilteredGraph & /*file_command_graph*/, const Context & /*context*/) const
{
    MSS_BEGIN(Result);

    const auto & files = recipe.files();
    auto it = files.find(LanguageTypePair(Language::Binary, Type::Library));

    if (it == files.end())
        MSS_RETURN_OK();

    for(const auto & file : it->second)
        MSS(add_derived_path(LanguageTypePair(Language::Binary, Type::LibraryPath), file, recipe));

    MSS_END();
}


} } }

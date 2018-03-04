#include "cook/process/souschef/IncludePathSetter.hpp"
#include "cook/process/souschef/PathExtraction.hpp"
#include <set>

namespace cook { namespace process { namespace souschef {

IncludePathSetter::IncludePathSetter(Language language)
    : language_(language)
{
    }

Result IncludePathSetter::process(model::Recipe & recipe, RecipeFilteredGraph & /*file_command_graph*/, const Context & /*context*/) const
{
    MSS_BEGIN(Result);

    auto & files = recipe.files();

    for(const auto & file : files.range(LanguageTypePair(language_, Type::Header)))
        MSS(add_derived_path(LanguageTypePair(language_, Type::IncludePath), file, recipe));

    for(const auto & file : files.range(LanguageTypePair(Language::Undefined, Type::IncludePath)))
        MSS(files.insert_or_merge(LanguageTypePair(language_, Type::IncludePath), file));

    MSS_END();
}

} } }


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

    if (language_ == Language::ObjectiveCXX)
    {
        MSS(add_derived_paths(recipe, LanguageTypePair(Language::ObjectiveC, Type::Header), LanguageTypePair(Language::Undefined, Type::IncludePath)));
        MSS(add_derived_paths(recipe, LanguageTypePair(Language::CXX, Type::Header), LanguageTypePair(Language::Undefined, Type::IncludePath)));
        MSS(add_derived_paths(recipe, LanguageTypePair(Language::C, Type::Header), LanguageTypePair(Language::Undefined, Type::IncludePath)));
    }
    else if (language_ == Language::ObjectiveC)
    {
        MSS(add_derived_paths(recipe, LanguageTypePair(Language::C, Type::Header), LanguageTypePair(Language::Undefined, Type::IncludePath)));
    }
    else if (language_ == Language::CXX)
    {
        MSS(add_derived_paths(recipe, LanguageTypePair(Language::C, Type::Header), LanguageTypePair(Language::Undefined, Type::IncludePath)));
    }

    MSS(add_derived_paths(recipe, LanguageTypePair(language_, Type::Header), LanguageTypePair(Language::Undefined, Type::IncludePath)));
    MSS(add_derived_paths(recipe, LanguageTypePair(Language::Undefined, Type::Header), LanguageTypePair(Language::Undefined, Type::IncludePath)));

    if (language_ == Language::ObjectiveCXX)
    {
        MSS(add_derived_paths(recipe, LanguageTypePair(Language::ObjectiveC, Type::ForceInclude), LanguageTypePair(Language::Undefined, Type::IncludePath)));
        MSS(add_derived_paths(recipe, LanguageTypePair(Language::CXX, Type::ForceInclude), LanguageTypePair(Language::Undefined, Type::IncludePath)));
        MSS(add_derived_paths(recipe, LanguageTypePair(Language::C, Type::ForceInclude), LanguageTypePair(Language::Undefined, Type::IncludePath)));
    }
    else if (language_ == Language::ObjectiveC)
    {
        MSS(add_derived_paths(recipe, LanguageTypePair(Language::C, Type::ForceInclude), LanguageTypePair(Language::Undefined, Type::IncludePath)));
    }
    if (language_ == Language::CXX)
    {
        MSS(add_derived_paths(recipe, LanguageTypePair(Language::C, Type::ForceInclude), LanguageTypePair(Language::Undefined, Type::IncludePath)));
    }
    MSS(add_derived_paths(recipe, LanguageTypePair(language_, Type::ForceInclude), LanguageTypePair(Language::Undefined, Type::IncludePath)));
    MSS(add_derived_paths(recipe, LanguageTypePair(Language::Undefined, Type::ForceInclude), LanguageTypePair(Language::Undefined, Type::IncludePath)));

    MSS_END();
}

} } }


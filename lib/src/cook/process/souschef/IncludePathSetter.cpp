#include "cook/process/souschef/IncludePathSetter.hpp"
#include "cook/process/souschef/PathExtraction.hpp"
#include <set>

namespace cook { namespace process { namespace souschef {

IncludePathSetter::IncludePathSetter(Language language)
    : language_(language)
{

}

Result IncludePathSetter::process(const Context & context, model::Recipe & recipe) const
{
    MSS_BEGIN(Result);

    auto & files = recipe.files();
    auto it = files.find(LanguageTypePair(language_, Type::Header));

    if (it == files.end())
        MSS_RETURN_OK();

    for(const ingredient::File & file : it->second)
        MSS(add_derived_path(LanguageTypePair(language_, Type::IncludePath), file, recipe));

    MSS_END();
}

} } }

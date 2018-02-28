#include "cook/staff/souschef/IncludePathSetter.hpp"
#include "cook/staff/souschef/PathExtraction.hpp"
#include <set>

namespace cook { namespace staff { namespace souschef {

IncludePathSetter::IncludePathSetter(Language language)
    : language_(language)
{

}

Result IncludePathSetter::process(const Context & context, model::Snapshot & snapshot) const
{
    MSS_BEGIN(Result);

    auto & files = snapshot.files();
    auto it = files.find(LanguageTypePair(language_, Type::Header));

    if (it == files.end())
        MSS_RETURN_OK();

    for(const ingredient::File & file : it->second)
        MSS(add_derived_path(LanguageTypePair(language_, Type::IncludePath), file, snapshot, context.recipe));

    MSS_END();
}

} } }

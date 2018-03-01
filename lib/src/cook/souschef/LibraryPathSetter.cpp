#include "cook/souschef/LibraryPathSetter.hpp"
#include "cook/souschef/PathExtraction.hpp"
#include <set>

namespace cook { namespace souschef {

Result LibraryPathSetter::process(const Context & context, model::Snapshot & snapshot) const
{
    MSS_BEGIN(Result);

    const auto & files = snapshot.files();
    auto it = files.find(LanguageTypePair(Language::Binary, Type::Library));

    if (it == files.end())
        MSS_RETURN_OK();

    for(const auto & file : it->second)
        MSS(add_derived_path(LanguageTypePair(Language::Binary, Type::LibraryPath), file, snapshot, context.recipe));

    MSS_END();
}


} }

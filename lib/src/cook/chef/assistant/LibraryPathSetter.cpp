#include "cook/chef/assistant/LibraryPathSetter.hpp"
#include "cook/chef/assistant/PathExtraction.hpp"
#include <set>

namespace cook { namespace chef { namespace assistant {

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


} } }

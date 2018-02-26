#ifndef HEADER_cook_chef_assistant_PathExtraction_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_assistant_PathExtraction_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "cook/LanguageTypePair.hpp"
#include "cook/ingredient/File.hpp"
#include "cook/model/Snapshot.hpp"

namespace cook { namespace chef { namespace assistant {

inline Result add_derived_path(const LanguageTypePair & key, const ingredient::File & file, model::Snapshot & snapshot, model::Recipe * recipe)
{
    MSS_BEGIN(Result);

    // create a file object for the path
    ingredient::File path(file.dir(), {});

    path.set_overwrite(Overwrite::Always);
    path.set_owner(recipe);
    path.set_propagation( file.propagation() );

    MSS(snapshot.files().insert_or_merge(key, path));

    MSS_END();
}

} } }

#endif

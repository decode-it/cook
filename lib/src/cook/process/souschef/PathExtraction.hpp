#ifndef HEADER_cook_process_souschef_PathExtraction_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_souschef_PathExtraction_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "cook/LanguageTypePair.hpp"
#include "cook/ingredient/File.hpp"

namespace cook { namespace process { namespace souschef {

inline Result add_derived_path(const LanguageTypePair & key, const ingredient::File & file, model::Recipe & recipe)
{
    MSS_BEGIN(Result);

    // create a file object for the path
    ingredient::File path(file.dir(), {});

    path.set_overwrite(Overwrite::Always);
    path.set_owner(&recipe);
    path.set_propagation( file.propagation() );

    MSS(recipe.files().insert_or_merge(key, path));

    MSS_END();
}

} } }

#endif

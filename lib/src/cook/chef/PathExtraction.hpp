#ifndef HEADER_cook_chef_PathExtraction_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_PathExtraction_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "cook/LanguageTypePair.hpp"
#include "cook/ingredient/File.hpp"
#include "cook/model/Snapshot.hpp"

namespace cook { namespace chef {

template <typename FileSelection>
Result add_derived_path(FileSelection file_selector, const model::Snapshot & source_snapshot, model::Snapshot & destination, model::Recipe * recipe)
{
    MSS_BEGIN(Result);

    source_snapshot.files().each ([&](const LanguageTypePair & key, const ingredient::File & file) {

        MSS_BEGIN(Result);

        auto res = file_selector(key);
        if (!res)
            MSS_RETURN_OK();

        // create a file object for the path
        ingredient::File path(file.dir(), {});

        path.set_overwrite(Overwrite::Always);
        path.set_owner(recipe);
        path.set_propagation( file.propagation() );

        MSS(destination.files().insert_or_merge(*res, path));

        MSS_END();

    });

    MSS_END();
}

} }

#endif

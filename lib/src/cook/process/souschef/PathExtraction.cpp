#include "cook/process/souschef/PathExtraction.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace process { namespace souschef {

Result add_derived_path(const LanguageTypePair & key, const ingredient::File & file, model::Recipe & recipe)
{
    MSS_BEGIN(Result);

    if (file.dir().empty())
        MSS_RETURN_OK();

    // create a file object for the path
    ingredient::File path(file.dir(), {});
    path.set_content(Content::Generated);

    path.set_overwrite(Overwrite::Always);
    path.set_owner(&recipe);
    path.set_propagation( file.propagation() );

    MSS(recipe.files().insert_or_merge(key, path));

    MSS_END();
}

Result add_derived_paths(model::Recipe & recipe, const LanguageTypePair & src_pair, const LanguageTypePair & dst_pair)
{
    MSS_BEGIN(Result);

    std::map<std::filesystem::path, Propagation> dirs;

    for(const auto & file : recipe.files().range(src_pair))
    {
        const std::filesystem::path d = file.dir();
        auto it = dirs.insert(std::make_pair(d, file.propagation())).first;
        MSS(merge(it->second, file.propagation()));
    }

    for (const auto & p : dirs)
    {
        // create a file object for the path
        ingredient::File dir(p.first, {});
        dir.set_content(Content::Generated);
        dir.set_overwrite(Overwrite::Always);
        dir.set_owner(&recipe);
        dir.set_propagation( p.second );

        MSS(recipe.files().insert_or_merge(dst_pair, dir));
    }

    MSS_END();
}

} } }

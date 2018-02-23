#include "cook/chef/IncludePath.hpp"
#include <set>

namespace cook { namespace chef {

IncludePath::IncludePath(const SelectionFunction selection_functor)
    : selection_(selection_functor)
{
}

bool IncludePath::process(const Context & context, model::Snapshot & snapshot, model::Snapshot & /*post*/) const
{
    MSS_BEGIN(bool);

    std::map<Language, std::set<std::filesystem::path> > include_paths;

    // find all include paths
    {
        auto find_paths = [&](const LanguageTypePair & key, const ingredient::File & file)
        {
            if (key.type == Type::Header)
                if (selection_ && selection_(key.language))
                    include_paths[key.language].insert(file.dir());

            return true;
        };
        MSS(snapshot.files().each(find_paths));
    }

    for(const auto & p : include_paths)
    {
        const LanguageTypePair key(p.first, Type::IncludePath);

        for(const auto & dir : p.second)
        {
            auto it_p = snapshot.files().insert(key, ingredient::File(dir, {}));
            ingredient::File & include_path = *it_p.first;

            // set the owner
            if (it_p.second)
                include_path.set_owner(context.recipe);

            include_path.set_overwrite(Overwrite::Always);
            include_path.set_propagation(Propagation::Public);
        }
    }


    MSS_END();
}

} }

#include "cook/agents/LibraryPath.hpp"

namespace cook { namespace agents {

LibraryPath::LibraryPath(const SelectionFunctor & selection_functor)
    : selection_(selection_functor)
{
}

bool LibraryPath::process(const Context & context, model::Snapshot & snapshot, model::Snapshot & /*post*/) const
{
    MSS_BEGIN(bool);

    using LibraryPath = std::pair<cook::Propagation, std::filesystem::path>;

    std::map<Language, std::set<LibraryPath> > library_paths;

    // find all libraries
    {
        auto find_paths = [&](const LanguageTypePair & key, const property::File & file)
        {
            if (key.type == Type::Library)
                if (selection_ && selection_(key.language))
                    library_paths[key.language].insert(std::make_pair(file.propagation(), file.dir()));

            return true;
        };
        MSS(snapshot.file_properties().each(find_paths));
    }

    for(const auto & lp : library_paths)
    {
        const LanguageTypePair key(lp.first, Type::LibraryPath);

        for(const LibraryPath & p : lp.second)
        {
            auto it_p = snapshot.file_properties().insert(key, property::File(p.second, {}));
            property::File & library_path = *it_p.first;

            // set the owner
            if (it_p.second)
                library_path.set_owner(context.recipe);

            library_path.set_propagation(std::max(p.first, library_path.propagation()));
            library_path.set_overwrite(Overwrite::Always);
        }
    }


    MSS_END();
}

} }

#include "cook/process/souschef/Archiver.hpp"
#include "cook/process/command/gcclike/Archiver.hpp"
#include "cook/util/File.hpp"
#include "gubg/stream.hpp"


namespace cook { namespace process { namespace souschef {

Result Archiver::process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const
{
    MSS_BEGIN(Result);

    log::Importance importance{0};
    auto ss = log::scope("Archiver::process", importance);

    model::Recipe::Files & files = recipe.files();
    auto & g = file_command_graph;

    auto objects = files.range(LanguageTypePair(Language::Binary, Type::Object));
    if (objects.empty())
    {
        MSS_RC << MESSAGE(Warning, "archive for " << recipe.uri() << " is not created as it contains no object code");
        MSS_RETURN_OK();
    }

    auto archive_vertex = g.add_vertex(archive_command(recipe, context));

    // stop the propagation, this file is contained in the library
    for(ingredient::File & object : objects)
    {
        object.set_propagation(Propagation::Private);
        auto ss = log::scope("object", importance, [&](auto &node){node.attr("file", object);});
        MSS(g.add_edge(archive_vertex, g.goc_vertex(object.key())));
    }

    // get the library dir (local to the path)
    const std::filesystem::path & library_dir = util::make_local_to_recipe(util::make_recipe_adj_path(recipe), context.dirs().output());
    {
        MSS(!!recipe.build_target().filename);
        ingredient::File dep(library_dir, *recipe.build_target().filename);

        dep.set_overwrite(Overwrite::IfSame);
        dep.set_owner(&recipe);
        dep.set_propagation(Propagation::Public);
        const LanguageTypePair key(Language::Binary, Type::Dependency);
        MSG_MSS(files.insert(key,dep).second, Error, "Dependency " << dep << " already present in " << recipe.uri());

        // add the link in the execution graph
        {
            const std::filesystem::path & lib_fn = util::make_global_from_recipe(recipe, dep.key());
            MSS(g.add_edge(g.goc_vertex(lib_fn), archive_vertex));
        }
    }

    // add the library type
    {
        ingredient::KeyValue ar(recipe.build_target().name);
        ar.set_overwrite(Overwrite::IfSame);
        ar.set_owner(&recipe);
        ar.set_propagation(Propagation::Public);

        const LanguageTypePair key(Language::Binary, Type::Library);
        MSG_MSS(recipe.key_values().insert(key,ar).second, Error, "Archive " << ar << " already present in " << recipe.uri());
    }

    // add the library dir
    {
        ingredient::File ar(library_dir, {});
        ar.set_overwrite(Overwrite::IfSame);
        ar.set_owner(&recipe);
        ar.set_propagation(Propagation::Public);
        const LanguageTypePair key(Language::Binary, Type::LibraryPath);
        files.insert(key,ar);
    }

    MSS_END();
}


command::Ptr Archiver::archive_command(const model::Recipe & recipe, const Context & context) const
{
    return std::make_shared<command::gcclike::Archiver>();
}

} } }

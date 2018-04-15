#include "cook/process/souschef/Archiver.hpp"
#include "cook/process/command/gcclike/Archiver.hpp"
#include "gubg/stream.hpp"

namespace cook { namespace process { namespace souschef {

Result Archiver::process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const
{
    MSS_BEGIN(Result);

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
        MSS(g.add_edge(archive_vertex, g.goc_vertex(object.key())));
    }

    // add the dependency to this graph
    {
        MSS(!!recipe.build_target().filename);
        ingredient::File dep(context.dirs().output(), *recipe.build_target().filename);
        dep.set_overwrite(Overwrite::IfSame);
        dep.set_owner(&recipe);
        dep.set_propagation(Propagation::Public);
        const LanguageTypePair key(Language::Binary, Type::Dependency);
        MSG_MSS(files.insert(key,dep).second, Error, "Dependency " << dep << " already present in " << recipe.uri());

        // add the link in the execution graph
        MSS(g.add_edge(g.goc_vertex(dep.key()), archive_vertex));
    }


    // add the library
    {
        ingredient::File ar("", recipe.build_target().name);
        ar.set_overwrite(Overwrite::IfSame);
        ar.set_owner(&recipe);
        ar.set_propagation(Propagation::Public);
        const LanguageTypePair key(Language::Binary, Type::Library);
        MSG_MSS(files.insert(key,ar).second, Error, "Archive " << ar << " already present in " << recipe.uri());
    }

    // add the library
    {
        ingredient::File ar(context.dirs().output(), "");
        ar.set_overwrite(Overwrite::Always);
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

#include "cook/process/souschef/Linker.hpp"
#include "cook/process/command/gcclike/Linker.hpp"
#include "gubg/stream.hpp"
#include "boost/predef.h"

namespace cook { namespace process { namespace souschef {

Result Linker::process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const
{
    MSS_BEGIN(Result);

    model::Recipe::Files & files = recipe.files();
    auto & g = file_command_graph;

    auto objects = files.range(LanguageTypePair(Language::Binary, Type::Object));
    auto libs = files.range(LanguageTypePair(Language::Binary, Type::Library));
    auto deps = files.range(LanguageTypePair(Language::Binary, Type::Dependency));

    if (objects.empty() && libs.empty())
    {
        MSS_RC << MESSAGE(Warning, "archive for " << recipe.uri() << " is not created as it contains no object code");
        MSS_RETURN_OK();
    }

    const auto link_vertex = g.add_vertex(link_command(recipe, context));

    // link the objects
    for(ingredient::File & object : objects)
    {
        // stop the propagation, this file is contained in the library
        object.set_propagation(Propagation::Private);
        MSS(g.add_edge(link_vertex, g.goc_vertex(object.key())));
    }

    // link the libraries
    for(ingredient::File & lib: libs)
    {
        // stop the propagation, this file is contained in the library
        lib.set_propagation(Propagation::Private);
    }

    for(ingredient::File & dep : deps)
    {
        // stop the propagation, this file is contained in the library
        dep.set_propagation(Propagation::Private);

        MSS(g.add_edge(link_vertex, g.goc_vertex(dep.key()), RecipeFilteredGraph::Implicit));
    }

    // create the archive
    MSS(!!recipe.build_target().filename, "No filename has been set for this build target");
    const ingredient::File archive = construct_archive_file(recipe, context);
    const LanguageTypePair key(Language::Binary, Type::Library);
    MSG_MSS(files.insert(key,archive).second, Error, "Archive " << archive << " already present in " << recipe.uri());

    // add the link in the execution graph
    MSS(g.add_edge(g.goc_vertex(archive.key()), link_vertex));

    MSS_END();
}

ingredient::File Linker::construct_archive_file(model::Recipe & recipe, const Context &context) const
{
    const std::filesystem::path dir = context.dirs().output();
    const std::filesystem::path rel = *recipe.build_target().filename;

    ingredient::File archive(dir, rel);
    archive.set_overwrite(Overwrite::IfSame);
    archive.set_owner(&recipe);
    archive.set_propagation(Propagation::Public);

    return archive;
}

command::Ptr Linker::link_command(const model::Recipe & recipe, const Context & context) const
{
    std::shared_ptr<command::Linker> ptr = std::make_shared<command::gcclike::Linker>();

    // set the libraries
    for(const ingredient::File & lib: recipe.files().range(LanguageTypePair(Language::Binary, Type::Library)))
       ptr->add_library(lib.rel());

    // set the library paths
    for(const ingredient::File & lib: recipe.files().range(LanguageTypePair(Language::Binary, Type::LibraryPath)))
       ptr->add_library_path(lib.dir());


    return ptr;
}

} } }

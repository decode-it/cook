#include "cook/process/souschef/Linker.hpp"
#include "gubg/stream.hpp"
#include "boost/predef.h"

namespace cook { namespace process { namespace souschef {

namespace  {

struct DummyLinker : public build::Command
{
    std::string name() const override { return "link"; }
    Result process(const std::list<std::filesystem::path> & input, const std::list<std::filesystem::path> & output) override
    {
        return Result();
    }
};

std::string construct_dynamic_library_name(const model::Recipe & recipe)
{
#if BOOST_OS_WINDOWS
    return gubg::stream([&](auto & os) { os << recipe.uri().string('.') << ".dll"; });
#else
    return gubg::stream([&](auto & os) { os << "lib" << recipe.uri().string('.') << ".so"; });
#endif
}

std::string construct_executable_name(const model::Recipe & recipe)
{
#if BOOST_OS_WINDOWS
    return gubg::stream([&](auto & os) { os << recipe.uri().string('.') << ".exe"; });
#else
    return gubg::stream([&](auto & os) { os << recipe.uri().string('.'); });
#endif
}


}

Result Linker::process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const
{
    MSS_BEGIN(Result);

    model::Recipe::Files & files = recipe.files();
    auto & g = file_command_graph;

    auto objects = files.range(LanguageTypePair(Language::Binary, Type::Object));
    auto libs = files.range(LanguageTypePair(Language::Binary, Type::Library));

    if (objects.empty() && libs.empty())
    {
        MSS_RC << MESSAGE(Warning, "archive for " << recipe.uri() << " is not created as it contains no object code");
        MSS_RETURN_OK();
    }

    const auto link_vertex = g.add_vertex(link_command(context));

    // link the objects
    for(ingredient::File & object : objects)
    {
        // stop the propagation, this file is contained in the library
        object.set_propagation(Propagation::Private);
        MSS(g.add_edge(g.goc_vertex(object.key()), link_vertex));
    }

    // link the libraries
    for(ingredient::File & lib: libs)
    {
        // stop the propagation, this file is contained in the library
        lib.set_propagation(Propagation::Private);

        if (lib.owner() != nullptr)
            MSS(g.add_edge(g.goc_vertex(lib.key()), link_vertex));
    }

    // create the archive
    const ingredient::File archive = construct_archive_file(recipe, context);
    const LanguageTypePair key(Language::Binary, Type::Library);
    MSG_MSS(files.insert(key,archive).second, Error, "Archive " << archive << " already present in " << recipe.uri());

    // add the link in the execution graph
    MSS(g.add_edge(link_vertex, g.goc_vertex(archive.key())));

    MSS_END();
}

ingredient::File Linker::construct_archive_file(model::Recipe & recipe, const Context &context) const
{
    const std::filesystem::path dir = context.dirs().output() / recipe.working_directory();
    const std::filesystem::path rel = (recipe.type() == Type::Executable ? construct_executable_name(recipe) : construct_dynamic_library_name(recipe));

    ingredient::File archive(dir, rel);
    archive.set_overwrite(Overwrite::IfSame);
    archive.set_owner(&recipe);
    archive.set_propagation(Propagation::Public);

    return archive;
}

build::CommandPtr Linker::link_command(const Context & context) const
{
    return std::make_shared<DummyLinker>();
}

} } }

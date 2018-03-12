#include "cook/process/souschef/Compiler.hpp"

namespace cook { namespace process { namespace souschef {

namespace  {

struct DummyCompiler : public build::Command
{
    std::string name() const override { return "compile"; }
    Result process(const std::list<std::filesystem::path> & input, const std::list<std::filesystem::path> & output) override
    {
        return Result();
    }
};

}

Compiler::Compiler(Language language)
    : language_(language)
{

}

Result Compiler::process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const
{
    MSS_BEGIN(Result);
    L(C(&file_command_graph)C(file_command_graph.num_vertices()));

    auto & g = file_command_graph;

    model::Recipe::Files & files = recipe.files();

    auto it = files.find(LanguageTypePair(language_, Type::Source));
    if (it == files.end())
        MSS_RETURN_OK();
    L("Found source files for " << language_);

    build::CommandPtr cc = compile_command(context);

    for (const ingredient::File & source : it->second)
    {
        MSG_MSS(source.propagation() == Propagation::Private, Warning, "Source file '" << source << "' in " << recipe.uri() << " has public propagation and will (probably) result into multiple defined symbols");

        const ingredient::File & object = construct_object_file(source, recipe, context);
        const LanguageTypePair key(Language::Binary, Type::Object);

        MSG_MSS(files.insert(key, object).second, Error, "Object file '" << object << "' already present in " << recipe.uri());

        L("Adding compile command");
        auto compile_vertex = g.add_vertex(cc);
        {
            auto source_vertex = g.goc_vertex(source.key());
            MSS(g.add_edge(source_vertex, compile_vertex));
        }
        {
            auto object_vertex = g.goc_vertex(object.key());
            MSS(g.add_edge(compile_vertex, object_vertex));
        }
    }

    MSS_END();
}

ingredient::File Compiler::construct_object_file(const ingredient::File & source, model::Recipe & recipe, const Context & context) const
{
    const std::filesystem::path dir = gubg::filesystem::combine(context.dirs().temporary(), source.dir());
    const std::filesystem::path rel = source.rel().string() + ".obj";

    ingredient::File object(dir, rel);
    object.set_owner(&recipe);
    object.set_overwrite(Overwrite::Never);
    object.set_propagation(Propagation::Public);

    return object;
}

build::CommandPtr Compiler::compile_command(const Context & context) const
{
    return std::make_shared<DummyCompiler>();
}

} } }

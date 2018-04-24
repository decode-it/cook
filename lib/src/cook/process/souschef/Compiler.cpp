#include "cook/process/souschef/Compiler.hpp"
#include "cook/process/command/gcclike/Compiler.hpp"
#include "cook/util/File.hpp"
#include "cook/log/Scope.hpp"

namespace cook { namespace process { namespace souschef {

Compiler::Compiler(Language language)
    : language_(language)
{
}

Result Compiler::process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const
{
    MSS_BEGIN(Result);
    log::Importance importance{0};
    auto ss = log::scope("Compiler::process", importance);
    L(C(&file_command_graph)C(file_command_graph.num_vertices()));

    auto & g = file_command_graph;

    model::Recipe::Files & files = recipe.files();

    for(auto & file : files.range(LanguageTypePair(language_, Type::Header)))
        file.set_propagation(Propagation::Private);

    auto it = files.find(LanguageTypePair(language_, Type::Source));
    if (it == files.end())
        MSS_RETURN_OK();
    L("Found source files for " << language_);

    command::Ptr cc = compile_command(recipe, context);

    //Relative adjustment path from recipe.working_directory() to root
    const std::filesystem::path & adj = util::make_recipe_adj_path(recipe);

    for (const ingredient::File & source : it->second)
    {
        MSG_MSS(source.propagation() == Propagation::Private, Warning, "Source file '" << source << "' in " << recipe.uri() << " has public propagation and will (probably) result into multiple defined symbols");

        const ingredient::File object = construct_object_file(source, recipe, context, adj);
        auto ss = log::scope("Compiler::process", importance, [&](auto &node){node.attr("source", source).attr("object", object);});
        const LanguageTypePair key(Language::Binary, Type::Object);

        MSG_MSS(files.insert(key, object).second, Error, "Object file '" << object << "' already present in " << recipe.uri());

        L("Adding compile command");
        auto compile_vertex = g.add_vertex(cc);
        {
            const std::filesystem::path & src_fn = util::make_global_from_recipe(recipe, source.key());
            auto source_vertex = g.goc_vertex(src_fn);
            MSS(g.add_edge(compile_vertex, source_vertex));
        }
        {
            auto object_vertex = g.goc_vertex(object.key());
            MSS(g.add_edge(object_vertex, compile_vertex));
        }
    }

    MSS_END();
}

ingredient::File Compiler::construct_object_file(const ingredient::File & source, model::Recipe & recipe, const Context & context, const std::filesystem::path & adj_path) const
{
    auto ss = log::scope("construct_object_file");
    const std::filesystem::path dir = util::make_local_to_recipe(adj_path, gubg::filesystem::normalize(context.dirs().temporary() / recipe.working_directory() /source.dir()));
    /* const std::filesystem::path dir = gubg::filesystem::normalize(context.dirs().temporary() / recipe.working_directory() /source.dir()); */
    const std::filesystem::path rel = source.rel().string() + ".obj";

    ingredient::File object(dir, rel);
    object.set_owner(&recipe);
    object.set_overwrite(Overwrite::IfSame);
    object.set_propagation(Propagation::Public);

    return object;
}

command::Ptr Compiler::compile_command(const model::Recipe & recipe, const Context & context) const
{
    std::shared_ptr<command::Compiler> cp = std::make_shared<command::gcclike::Compiler>(language_);

    // add the include paths
    for (const ingredient::File & f : recipe.files().range(LanguageTypePair(Language::Undefined, Type::IncludePath)))
        cp->add_include_path(util::make_global_from_recipe(recipe, f.dir()));

    // add the defines
    for (const ingredient::KeyValue & f : recipe.key_values().range(LanguageTypePair(Language::Undefined, Type::Define)))
        if (f.has_value())
            cp->add_define(f.key(), f.value());
        else
            cp->add_define(f.key());

    return cp;
}

} } }

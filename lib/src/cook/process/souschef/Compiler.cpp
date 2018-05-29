#include "cook/process/souschef/Compiler.hpp"
#include "cook/process/toolchain/Manager.hpp"
#include "cook/util/File.hpp"
#include "cook/log/Scope.hpp"
#include "gubg/hash/MD5.hpp"

namespace cook { namespace process { namespace souschef {

Compiler::Compiler(Language language)
    : language_(language)
{
}

Result Compiler::process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const
{
    MSS_BEGIN(Result);
    auto ss = log::scope("Compiler::process", [&](auto &node){node.attr("graph", &file_command_graph);});
    L(C(&file_command_graph)C(file_command_graph.num_vertices()));

    auto & g = file_command_graph;

    model::Recipe::Files & files = recipe.files();

    for(auto & file : files.range(LanguageTypePair(language_, Type::Header)))
        file.set_propagation(Propagation::Private);

    auto it = files.find(LanguageTypePair(language_, Type::Source));
    if (it == files.end())
        MSS_RETURN_OK();
    L("Found source files for " << language_);

    command::Ptr cc;
    MSG_MSS(compile_command_(cc, recipe, context), Error, "Could not create compile command");

    for (const ingredient::File & source : it->second)
    {
        MSG_MSS(source.propagation() == Propagation::Private, Warning, "Source file '" << source << "' in " << recipe.uri() << " has public propagation and will (probably) result into multiple defined symbols");

        const ingredient::File object = construct_object_file(source, recipe, context);
        auto ss = log::scope("Compiler::process", [&](auto &node){node.attr("source", source).attr("object", object);});
        const LanguageTypePair key(Language::Binary, Type::Object);

        MSG_MSS(files.insert(key, object).second, Error, "Object file '" << object << "' already present in " << recipe.uri());

        L("Adding compile command");
        auto compile_vertex = g.add_vertex(cc);
        {
            const std::filesystem::path & src_fn = gubg::filesystem::combine({recipe.working_directory(), source.dir(), source.rel()});
            auto source_vertex = g.goc_vertex(src_fn);
            MSS(g.add_edge(compile_vertex, source_vertex));
        } 
        {
            const std::filesystem::path & obj_fn = gubg::filesystem::combine({recipe.working_directory(), object.dir(), object.rel()});
            auto object_vertex = g.goc_vertex(obj_fn);
            MSS(g.add_edge(object_vertex, compile_vertex));
        }
    }

    MSS_END();
}

ingredient::File Compiler::construct_object_file(const ingredient::File & source, model::Recipe & recipe, const Context & context) const
{
    auto ss = log::scope("construct_object_file");

    // get the path to the file
    std::filesystem::path p = context.dirs().temporary(true) / recipe.uri().string();
    {
        gubg::hash::md5::Stream s;
        s << source.dir().string();
        p /= s.hash_hex();
    }

    std::filesystem::path dir = util::get_from_to_path(recipe, p);
    const std::filesystem::path rel = source.rel().string() + ".obj";

    ingredient::File object(dir, rel);
    object.set_content(Content::Generated);
    object.set_owner(&recipe);
    object.set_overwrite(Overwrite::IfSame);
    object.set_propagation(Propagation::Public);

    return object;
}

Result Compiler::compile_command_(command::Ptr &ptr, const model::Recipe & recipe, const Context & context) const
{
    MSS_BEGIN(Result);
    auto adj = util::get_from_to_path("./", recipe);

    auto element = context.toolchain().element(toolchain::Element::Compile, language_, TargetType::Object);
    MSS(!!element);
    auto cp = element->create<process::command::Compile>();
    MSS(!!cp);

    // add the include paths
    for (const ingredient::File & f : recipe.files().range(LanguageTypePair(Language::Undefined, Type::IncludePath)))
        cp->add_include_path(gubg::filesystem::combine(adj, f.dir()));

    // add the force includes
    for (const ingredient::File & f : recipe.files().range(LanguageTypePair(language_, Type::ForceInclude)))
        //The necessary include path is already added by IncludePathSetter
        cp->add_force_include(f.rel());

    // add the defines
    for (const ingredient::KeyValue & f : recipe.key_values().range(LanguageTypePair(Language::Undefined, Type::Define)))
        if (f.has_value())
            cp->add_define(f.key(), f.value());
        else
            cp->add_define(f.key());

    cp->set_recipe_uri(recipe.uri().string());
    ptr = cp;

    MSS_END();
}

} } }

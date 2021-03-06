#include "cook/process/souschef/Compiler.hpp"
#include "cook/process/toolchain/Manager.hpp"
#include "cook/process/command/Compile.hpp"
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

        const model::Recipe::Files & files = recipe.files();

        recipe.each_file(LanguageTypePair(language_, Type::Header), [](auto &f) {
            f.set_propagation(Propagation::Private);
            return true;
        });

        auto it = recipe.files().find(LanguageTypePair(language_, Type::Source));
        if (it == files.end() || it->second.empty())
            MSS_RETURN_OK();
        L("Found source files for " << language_);

        command::Ptr cp;
        MSS(compile_command_(cp, recipe, context));
    

        for (const ingredient::File & source : it->second)
        {
            MSG_MSS(source.propagation() == Propagation::Private, Warning, "Source file '" << source << "' in " << recipe.uri() << " has public propagation and will (probably) result into multiple defined symbols");

            const ingredient::File object = construct_object_file(source, recipe, context);
            auto ss = log::scope("Compiler::process", [&](auto &node){node.attr("source", source).attr("object", object);});
            const LanguageTypePair key(Language::Binary, Type::Object);

            MSG_MSS(recipe.insert(key, object), Error, "Object file '" << object << "' already present in " << recipe.uri());

            L("Adding compile command");
            auto compile_vertex = g.add_vertex(cp);
            {
                const std::filesystem::path & src_fn = source.key();
                auto source_vertex = g.goc_vertex(src_fn);
                MSS(g.add_edge(compile_vertex, source_vertex));
            } 
            {
                const std::filesystem::path & obj_fn = object.key();
                auto object_vertex = g.goc_vertex(obj_fn);
                MSS(g.add_edge(object_vertex, compile_vertex));
            }
        }

        MSS_END();
    }

    ingredient::File Compiler::construct_object_file(const ingredient::File & source, model::Recipe & recipe, const Context & context) const
    {
        auto tmp_path = context.dirs().temporary(true);

        // get the path to the file, making sure the initial separator from the URI is dropped
        std::filesystem::path dir = tmp_path / recipe.uri().string(false);
        {
            gubg::hash::md5::Stream s;
            s << source.dir().string();
            dir /= s.hash_hex();
        }

        const std::filesystem::path rel = context.toolchain().intermediary_name(source.rel(), LanguageTypePair(language_, Type::Source), LanguageTypePair(Language::Binary, Type::Object), process::toolchain::Element::Compile);

        ingredient::File object(dir, rel);
        object.set_content(Content::Generated);
        object.set_owner(&recipe);
        object.set_overwrite(Overwrite::IfSame);
        object.set_propagation(Propagation::Public);

        return object;
    }
    
    Result Compiler::compile_command_(command::Ptr &ptr, model::Recipe & recipe, const Context & context) const
    {
        MSS_BEGIN(Result);
        
        ptr = context.toolchain().create_command<process::command::Compile>(toolchain::Element::Compile, language_, TargetType::Object, &recipe);
        MSS(!!ptr);

        MSS_END();
    }


} } }

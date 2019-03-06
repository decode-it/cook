#include "cook/process/souschef/Compiler.hpp"
#include "cook/process/toolchain/Manager.hpp"
#include "cook/util/File.hpp"
#include "cook/log/Scope.hpp"
#include "gubg/hash/MD5.hpp"

namespace cook { namespace process { namespace souschef {

    namespace { 

        void initialize_compile_command_(process::command::Compile & cp, const model::Recipe & recipe, Language language) 
        {
            // add the include paths
            auto adj = util::get_from_to_path("./", recipe);
            for (const ingredient::File & f : recipe.files().range(LanguageTypePair(Language::Undefined, Type::IncludePath)))
                cp.add_include_path(gubg::filesystem::combine(adj, f.dir()));

            // add the force includes
            for (const ingredient::File & f : recipe.files().range(LanguageTypePair(language, Type::ForceInclude)))
                //The necessary include path is already added by IncludePathSetter
                cp.add_force_include(f.rel());

            // add the defines
            for (const ingredient::KeyValue & f : recipe.key_values().range(LanguageTypePair(Language::Undefined, Type::Define)))
                if (f.has_value())
                    cp.add_define(f.key(), f.value());
                else
                    cp.add_define(f.key());

            cp.set_recipe_uri(recipe.uri().string());
        }
    }

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
        if (it == files.end() || it->second.empty())
            MSS_RETURN_OK();
        L("Found source files for " << language_);

        auto cp = context.toolchain().create_command<process::command::Compile>(toolchain::Element::Compile, language_, TargetType::Object, &recipe);
        if (!cp)
        {
            std::ostringstream oss;
            for (const ingredient::File & source : it->second)
                oss << std::endl << " * " << source;
            MSG_MSS(false, Error, "Could not find the toolchain compile element for language \"" << language_ << "\", either add such an element, or remove all files using this language:" << oss.str());
        }
        initialize_compile_command_(*cp, recipe, language_);

        for (const ingredient::File & source : it->second)
        {
            MSG_MSS(source.propagation() == Propagation::Private, Warning, "Source file '" << source << "' in " << recipe.uri() << " has public propagation and will (probably) result into multiple defined symbols");

            const ingredient::File object = construct_object_file(source, recipe, context);
            auto ss = log::scope("Compiler::process", [&](auto &node){node.attr("source", source).attr("object", object);});
            const LanguageTypePair key(Language::Binary, Type::Object);

            MSG_MSS(files.insert(key, object).second, Error, "Object file '" << object << "' already present in " << recipe.uri());

            L("Adding compile command");
            auto compile_vertex = g.add_vertex(cp);
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
        auto tmp_path = context.dirs().temporary(true);

        // get the path to the file, making sure the initial separator from the URI is dropped
        std::filesystem::path p = tmp_path / recipe.uri().string(false);
        {
            gubg::hash::md5::Stream s;
            s << source.dir().string();
            p /= s.hash_hex();
        }

        std::filesystem::path dir = util::get_from_to_path(recipe, p);
        const std::filesystem::path rel = context.toolchain().intermediary_name(source.rel(), LanguageTypePair(language_, Type::Source), LanguageTypePair(Language::Binary, Type::Object), process::toolchain::Element::Compile);

        ingredient::File object(dir, rel);
        object.set_content(Content::Generated);
        object.set_owner(&recipe);
        object.set_overwrite(Overwrite::IfSame);
        object.set_propagation(Propagation::Public);

        return object;
    }


} } }

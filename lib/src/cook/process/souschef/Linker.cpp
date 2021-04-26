#include "cook/process/souschef/Linker.hpp"
#include "cook/process/toolchain/Manager.hpp"
#include "cook/process/command/Link.hpp"
#include "cook/util/File.hpp"
#include "gubg/stream.hpp"

namespace cook { namespace process { namespace souschef {

    Result Linker::process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const
    {
        MSS_BEGIN(Result);

        auto ss = log::scope("Linker::process");

        const model::Recipe::Files & files = recipe.files();
        auto & g = file_command_graph;

        auto objects    = recipe.files().range(LanguageTypePair(Language::Binary, Type::Object));
        auto deps       = recipe.files().range(LanguageTypePair(Language::Binary, Type::Dependency));
        auto exports    = recipe.files().range(LanguageTypePair(Language::Definition, Type::Export));

        if (objects.empty() && deps.empty())
        {
            MSS_RC << MESSAGE(Warning, "archive for " << recipe.uri() << " is not created as it contains no object code");
            MSS_RETURN_OK();
        }

        // make the link vertex
        build::Graph::vertex_descriptor link_vertex;
        {
            command::Ptr lc;
            MSS(link_command_(lc, recipe, context));
            link_vertex = g.add_vertex(lc);
        }

        // link the objects
        for(const ingredient::File & object : objects)
        {
            auto ss = log::scope("object", [&](auto &node){node.attr("file", object);});
            const std::filesystem::path & obj_fn = object.key();
            MSS(g.add_edge(link_vertex, g.goc_vertex(obj_fn)));
        }

        // link the dependencies
        for(const ingredient::File & dep : deps)
        {
            const std::filesystem::path &fn = dep.key();
            MSS(g.add_edge(link_vertex, g.goc_vertex(fn), RecipeFilteredGraph::Implicit));
        }
        for(const ingredient::File & exp : exports)
        {
            const std::filesystem::path & fn = exp.key();
            MSS(g.add_edge(link_vertex, g.goc_vertex(fn), RecipeFilteredGraph::Implicit));
        }


        // create the local link dir
        const std::filesystem::path & library_dir = context.dirs().output(true);

        // create the linked file
        {
            MSG_MSS(!!recipe.build_target().filename, Error, "No filename has been set for build target " << recipe.uri());

            auto add_dep = [&]()
            {
                MSS_BEGIN(Result);

                ingredient::File dep(library_dir, *recipe.build_target().filename);
                dep.set_overwrite(Overwrite::IfSame);
                dep.set_owner(&recipe);
                dep.set_content(Content::Generated);
                dep.set_propagation(Propagation::Public);
                const LanguageTypePair key(Language::Binary, Type::Dependency);
                MSG_MSS(recipe.insert(key,dep), Error, "Dependency " << dep << " already present in " << recipe.uri());

                MSS_END();
            };

            auto add_dir = [&]()
            {
                    ingredient::File ar(library_dir, {});
                    ar.set_overwrite(Overwrite::IfSame);
                    ar.set_owner(&recipe);
                    ar.set_propagation(Propagation::Public);
                    ar.set_content(Content::Generated);
                    const LanguageTypePair key(Language::Binary, Type::LibraryPath);
                    recipe.insert(key,ar);
            };

            auto add_link = [&](Type t, Propagation propagation)
            {
                MSS_BEGIN(Result);

                ingredient::KeyValue ar(recipe.build_target().name);
                ar.set_overwrite(Overwrite::IfSame);
                ar.set_owner(&recipe);
                ar.set_propagation(propagation);
                ar.set_content(Content::Generated);

                const LanguageTypePair key(Language::Binary, t);
                MSG_MSS(recipe.insert(key,ar), Error, t << " " << ar << " already present in " << recipe.uri());
                MSS_END();
            };

            switch(recipe.build_target().type)
            {
                case TargetType::SharedLibrary:
                    MSS(add_dep());
                    add_dir();
                    MSS(add_link(Type::Library, Propagation::Public));
                    break;
                case TargetType::Plugin:
                    MSS(add_dep());
                    MSS(add_link(Type::Library, Propagation::Private));
                    break;
                case TargetType::Executable:
                    MSS(add_link(Type::Executable, Propagation::Public));
                    break;

                default:
                    MSG_MSS(false, Error, "Unknown build type " << recipe.build_target().type );
            }

            // add the link in the execution graph
            {
                const std::filesystem::path & lib_fn = context.dirs().output(true) / *recipe.build_target().filename;
                MSS(g.add_edge(g.goc_vertex(lib_fn), link_vertex));
            }
        }

        MSS_END();
    }

    ingredient::File Linker::construct_link_file(model::Recipe & recipe, const Context &context) const
    {
        const std::filesystem::path dir = context.dirs().output();
        const std::filesystem::path rel = *recipe.build_target().filename;

        ingredient::File link(dir, rel);
        link.set_content(Content::Generated);
        link.set_overwrite(Overwrite::IfSame);
        link.set_owner(&recipe);
        link.set_propagation(Propagation::Public);

        return link;
    }

    Result Linker::link_command_(command::Ptr &ptr, model::Recipe & recipe, const Context & context) const
    {
        MSS_BEGIN(Result);

        ptr = context.toolchain().create_command<process::command::Link>(toolchain::Element::Link, Language::Binary, recipe.build_target().type, &recipe);

        MSS_END();
    }

} } }

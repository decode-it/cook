#include "cook/process/souschef/Linker.hpp"
#include "cook/process/toolchain/Manager.hpp"
#include "cook/util/File.hpp"
#include "gubg/stream.hpp"

namespace cook { namespace process { namespace souschef {

    Result Linker::process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const
    {
        MSS_BEGIN(Result);

        auto ss = log::scope("Linker::process");

        model::Recipe::Files & files = recipe.files();
        auto & g = file_command_graph;

        auto stop_propagation = [](auto & ingredients)
        {
            for(auto & ingredient : ingredients)
                ingredient.set_propagation(Propagation::Private);
        };

        auto objects    = recipe.files().range(LanguageTypePair(Language::Binary, Type::Object));
        auto libs       = recipe.key_values().range(LanguageTypePair(Language::Binary, Type::Library));
        auto frameworks = recipe.key_values().range(LanguageTypePair(Language::Binary, Type::Framework));
        auto deps       = recipe.files().range(LanguageTypePair(Language::Binary, Type::Dependency));
        auto exports    = recipe.files().range(LanguageTypePair(Language::Definition, Type::Export));

        stop_propagation(objects);
        stop_propagation(libs);
        stop_propagation(frameworks);
        stop_propagation(deps);
        stop_propagation(exports);

        if (objects.empty() && deps.empty())
        {
            MSS_RC << MESSAGE(Warning, "archive for " << recipe.uri() << " is not created as it contains no object code");
            MSS_RETURN_OK();
        }

        // make the link vertex
        build::Graph::vertex_descriptor link_vertex;
        {
            command::Ptr lc;
            MSS(link_command_(lc, recipe, libs, frameworks, context));
            link_vertex = g.add_vertex(lc);
        }

        // link the objects
        for(ingredient::File & object : objects)
        {
            auto ss = log::scope("object", [&](auto &node){node.attr("file", object);});
            const std::filesystem::path & obj_fn = gubg::filesystem::combine({recipe.working_directory(), object.dir(), object.rel()});
            MSS(g.add_edge(link_vertex, g.goc_vertex(obj_fn)));
        }

        // link the dependencies
        for(ingredient::File & dep : deps)
        {
            const std::filesystem::path & fn = gubg::filesystem::combine({recipe.working_directory(), dep.dir(), dep.rel()});
            MSS(g.add_edge(link_vertex, g.goc_vertex(fn), RecipeFilteredGraph::Implicit));
        }
        for(ingredient::File & exp : exports)
        {
            const std::filesystem::path & fn = gubg::filesystem::combine({recipe.working_directory(), exp.dir(), exp.rel()});
            MSS(g.add_edge(link_vertex, g.goc_vertex(fn), RecipeFilteredGraph::Implicit));
        }


        // create the local link dir
        const std::filesystem::path & library_dir = util::get_from_to_path(recipe, context.dirs().output(true));

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
                MSG_MSS(files.insert(key,dep).second, Error, "Dependency " << dep << " already present in " << recipe.uri());

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
                    files.insert(key,ar);
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
                MSG_MSS(recipe.key_values().insert(key,ar).second, Error, t << " " << ar << " already present in " << recipe.uri());
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

        ingredient::File archive(dir, rel);
        archive.set_content(Content::Generated);
        archive.set_overwrite(Overwrite::IfSame);
        archive.set_owner(&recipe);
        archive.set_propagation(Propagation::Public);

        return archive;
    }

    Result Linker::link_command_(command::Ptr &ptr, model::Recipe & recipe, const model::Recipe::KeyValues::Range &libs, const model::Recipe::KeyValues::Range &frameworks, const Context & context) const
    {
        MSS_BEGIN(Result);

        auto lp = context.toolchain().create_command<process::command::Link>(toolchain::Element::Link, Language::Binary, recipe.build_target().type, &recipe);
        MSS(!!lp);

        auto adj = util::get_from_to_path(".", recipe);

        // set the libraries
        for(const ingredient::KeyValue & lib : libs)
            lp->add_library(lib.key());

        // set the frameworks
        for(const ingredient::KeyValue & framework : frameworks)
            lp->add_framework(framework.key());

        // set the library paths
        for(const ingredient::File & lib: recipe.files().range(LanguageTypePair(Language::Binary, Type::LibraryPath)))
            lp->add_library_path(gubg::filesystem::combine(adj, lib.dir()));

        // set the framework paths
        for(const ingredient::File & framework_path: recipe.files().range(LanguageTypePair(Language::Binary, Type::FrameworkPath)))
            lp->add_framework_path(gubg::filesystem::combine(adj, framework_path.dir()));

        // set the export files
        for(const ingredient::File & exp: recipe.files().range(LanguageTypePair(Language::Definition, Type::Export)))
            lp->add_export(gubg::filesystem::combine({adj, exp.dir(), exp.rel()}));

        lp->set_recipe_uri(recipe.uri().string());
        ptr = lp;

        MSS_END();
    }

} } }

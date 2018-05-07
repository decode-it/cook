#include "cook/process/souschef/Linker.hpp"
#include "cook/process/toolchain/Manager.hpp"
#include "cook/util/File.hpp"
#include "gubg/stream.hpp"
#include "boost/predef.h"

namespace cook { namespace process { namespace souschef {

Result Linker::process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const
{
    MSS_BEGIN(Result);

    auto ss = log::scope("Linker::process");

    model::Recipe::Files & files = recipe.files();
    auto & g = file_command_graph;

    auto stop_progagation = [](auto & ingredients)
    {
        for(auto & ingredient : ingredients)
            ingredient.set_propagation(Propagation::Private);
    };

    auto objects    = recipe.files().range(LanguageTypePair(Language::Binary, Type::Object));
    auto libs       = recipe.key_values().range(LanguageTypePair(Language::Binary, Type::Library));
    auto deps       = recipe.files().range(LanguageTypePair(Language::Binary, Type::Dependency));

    stop_progagation(objects);
    stop_progagation(libs);
    stop_progagation(deps);

    if (objects.empty() && deps.empty())
    {
        MSS_RC << MESSAGE(Warning, "archive for " << recipe.uri() << " is not created as it contains no object code");
        MSS_RETURN_OK();
    }

    // make the link vertex
    build::Graph::vertex_descriptor link_vertex;
    {
        command::Ptr lc;
        MSS(link_command_(lc, recipe, libs, context));
        link_vertex = g.add_vertex(lc);
    }

    // link the objects
    for(ingredient::File & object : objects)
    {
        auto ss = log::scope("object", [&](auto &node){node.attr("file", object);});
        const std::filesystem::path & obj_fn = util::make_global_from_recipe(recipe, object.key());
        MSS(g.add_edge(link_vertex, g.goc_vertex(obj_fn)));
    }

    // link the dependencies
    for(ingredient::File & dep : deps)
    {
        const std::filesystem::path & fn = util::make_global_from_recipe(recipe, dep.key());
        MSS(g.add_edge(link_vertex, g.goc_vertex(fn), RecipeFilteredGraph::Implicit));
    }


    // create the local link dir
    const std::filesystem::path & library_dir = util::make_local_to_recipe(util::make_recipe_adj_path(recipe), context.dirs().output());

    // create the linked file
    {
        MSG_MSS(!!recipe.build_target().filename, Error, "No filename has been set for build target " << recipe.uri());

        // only if it is an shared library
        if(recipe.build_target().type == TargetType::SharedLibrary)
        {
            // add a dependency to the project
            {
                ingredient::File dep(library_dir, *recipe.build_target().filename);
                dep.set_overwrite(Overwrite::IfSame);
                dep.set_owner(&recipe);
                dep.set_propagation(Propagation::Public);
                const LanguageTypePair key(Language::Binary, Type::Dependency);
                MSG_MSS(files.insert(key,dep).second, Error, "Dependency " << dep << " already present in " << recipe.uri());
            }

            // add the link include path
            {
                ingredient::File ar(library_dir, {});
                ar.set_overwrite(Overwrite::IfSame);
                ar.set_owner(&recipe);
                ar.set_propagation(Propagation::Public);
                const LanguageTypePair key(Language::Binary, Type::LibraryPath);
                files.insert(key,ar);
            }
        }

        // add the link in the execution graph
        {
            const std::filesystem::path & lib_fn = util::make_global_from_recipe(recipe, library_dir) / *recipe.build_target().filename;
            MSS(g.add_edge(g.goc_vertex(lib_fn), link_vertex));
        }

        // add the link type
        {
            ingredient::KeyValue ar(recipe.build_target().name);
            ar.set_overwrite(Overwrite::IfSame);
            ar.set_owner(&recipe);
            ar.set_propagation(Propagation::Public);

            Type t;
            switch(recipe.build_target().type)
            {
            case TargetType::Executable:
                t = Type::Executable;
                break;

            case TargetType::SharedLibrary:
                t = Type::Library;
                break;

            default:
                MSG_MSS(false, Error, "Unknown link type " << recipe.build_target().type);
            }


            const LanguageTypePair key(Language::Binary, t);
            MSG_MSS(recipe.key_values().insert(key,ar).second, Error, t << " " << ar << " already present in " << recipe.uri());
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

Result Linker::link_command_(command::Ptr &ptr, const model::Recipe & recipe, const model::Recipe::KeyValues::Range &libs, const Context & context) const
{
    MSS_BEGIN(Result);

    command::Link::Ptr lp;
    MSS(context.toolchain().linker().create(lp));

    // set the libraries
    for(const ingredient::KeyValue & lib : libs)
        lp->add_library(lib.key());

    // set the library paths
    for(const ingredient::File & lib: recipe.files().range(LanguageTypePair(Language::Binary, Type::LibraryPath)))
        lp->add_library_path(util::make_global_from_recipe(recipe, lib.dir()));

    lp->set_recipe_uri(recipe.uri().string());
    ptr = lp;

    MSS_END();
}

} } }

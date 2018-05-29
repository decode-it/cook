#include "cook/process/souschef/Archiver.hpp"
#include "cook/process/toolchain/Manager.hpp"
#include "cook/util/File.hpp"
#include "gubg/stream.hpp"

namespace cook { namespace process { namespace souschef {

Result Archiver::process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const
{
    MSS_BEGIN(Result);

    auto ss = log::scope("Archiver::process", [&](auto &node){node.attr("graph", &file_command_graph);});

    model::Recipe::Files & files = recipe.files();
    auto & g = file_command_graph;

    auto objects = files.range(LanguageTypePair(Language::Binary, Type::Object));
    if (objects.empty())
    {
        MSS_RC << MESSAGE(Warning, "archive for " << recipe.uri() << " is not created as it contains no object code");
        MSS_RETURN_OK();
    }

    command::Ptr ac;
    MSS(archive_command_(ac, recipe, context));
    auto archive_vertex = g.add_vertex(ac);

    // stop the propagation, this file is contained in the library
    for(ingredient::File & object : objects)
    {
        object.set_propagation(Propagation::Private);
        auto ss = log::scope("object", [&](auto &node){node.attr("file", object);});
        const std::filesystem::path & obj_fn = gubg::filesystem::combine({ recipe.working_directory(), object.dir(), object.rel() });
        MSS(g.add_edge(archive_vertex, g.goc_vertex(obj_fn)));
    }

    // get the library dir (local to the path)
    std::filesystem::path library_dir = util::get_from_to_path(recipe, context.dirs().output(true));
    {
        MSS(!!recipe.build_target().filename);
        ingredient::File dep(library_dir, *recipe.build_target().filename);
        dep.set_content(Content::Generated);
        dep.set_overwrite(Overwrite::IfSame);
        dep.set_owner(&recipe);
        dep.set_propagation(Propagation::Public);
        const LanguageTypePair key(Language::Binary, Type::Dependency);
        MSG_MSS(files.insert(key,dep).second, Error, "Dependency " << dep << " already present in " << recipe.uri());

        // add the link in the execution graph
        {
            const std::filesystem::path & lib_fn = context.dirs().output(true) / dep.key();
            MSS(g.add_edge(g.goc_vertex(lib_fn), archive_vertex));
        }
    }

    // add the library type
    {
        ingredient::KeyValue ar(recipe.build_target().name);
        ar.set_overwrite(Overwrite::IfSame);
        ar.set_owner(&recipe);
        ar.set_propagation(Propagation::Public);
        ar.set_content(Content::Generated);

        const LanguageTypePair key(Language::Binary, Type::Library);
        MSG_MSS(recipe.key_values().insert(key,ar).second, Error, "Archive " << ar << " already present in " << recipe.uri());
    }

    // add the library dir
    {
        ingredient::File ar(library_dir, {});
        ar.set_overwrite(Overwrite::IfSame);
        ar.set_owner(&recipe);
        ar.set_propagation(Propagation::Public);
        ar.set_content(Content::Generated);
        const LanguageTypePair key(Language::Binary, Type::LibraryPath);
        files.insert(key,ar);
    }

    MSS_END();
}

Result Archiver::archive_command_(command::Ptr &ptr, const model::Recipe & recipe, const Context & context) const
{
    MSS_BEGIN(Result);

    auto element = context.toolchain().element(toolchain::Element::Archive, Language::Binary, recipe.build_target().type);
    MSS(!!element);
    auto ap = element->create<process::command::Archive>();
    MSS(!!ap);
    
    ap->set_recipe_uri(recipe.uri().string());
    ptr = ap;
    MSS_END();
}

} } }

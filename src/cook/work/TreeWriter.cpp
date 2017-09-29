#include "cook/work/TreeWriter.hpp"

namespace cook { namespace work {

template <typename TGTFunctor>
bool write_recipe(const structure::Recipe & recipe, util::TreeNode & node, const Recipes & recipes, bool details, TGTFunctor && functor)
{
    MSS_BEGIN(bool);
    auto nn = node.open("recipe");

    // properties
    {
        nn.attr("uri", recipe.uri());
        nn.attr("display_name", recipe.display_name());
        if (details)
        {
            nn.attr("script", recipe.script_filename().native());
            nn.attr("type", recipe.target_type());
            nn.attr("build_target", recipe.output().filename.native());
        }
    }

    if (details)
    {
        // streaming of files
        {
            auto to_s = [](structure::file::Type type)
            {
                switch (type)
                {
                    case structure::file::Source: return "source"; break;
                    case structure::file::Header: return "header"; break;
                    case structure::file::ForceInclude: return "force_include"; break;

                }
                return "";

            };

            auto stream_files = [&](const auto & file_info)
            {
                for (const auto &p: file_info)
                    nn.open("file").attr("type", to_s(p.second.type)).attr("path", p.first.native());
            };

            stream_files(recipe.sources());
            stream_files(recipe.headers());
        }

        // extra material, coming from the dependencies
        structure::TargetConfig cfg;
        cfg = recipe.input_config();

        {
            std::list<structure::Recipe *> suborder;
            subset_order(std::back_inserter(suborder), recipe.uri(), util::make_range(recipes));

            for (auto it = suborder.begin(); it != suborder.end(); ++it)
            {
                auto * dep = *it;
                structure::merge(cfg, dep->output());
            }
        }

        // streaming of include paths
        {
            for (const auto & f : cfg.include_paths)
                nn.open("include_path").attr("path", f.native());
        }

        // streaming of defines
        {
            for (const auto & p : cfg.defines)
            {
                const auto &name = p.first;
                const auto &value = p.second;

                auto nnn = nn.open("define");
                nnn.attr("name", name);
                if (!value.empty())
                    nnn.attr("value", value);
            }
        }
    }

    MSS_END();
}
    
    bool TreeWriter::recipes(const Recipes &recipes)
    {         
        MSS_BEGIN(bool);
        
        auto n = util::make_tree_node(std::cout, "recipes");

        auto wrapper = [](const std::filesystem::path & p) { return p; };
        
        for (auto *recipe: recipes)
            MSS(write_recipe(*recipe, n, recipes, false, wrapper));

        if (!recipes.empty())
        {
            auto nn = n.open("default");
            nn.attr("uri", recipes.back()->uri());
        }
        
        MSS_END();
    }

    bool TreeWriter::details(const Recipes &recipes, const structure::Uri &uri, const std::filesystem::path & build_dir)
    {         
        MSS_BEGIN(bool);
        
        auto n = util::make_tree_node(std::cout, "details");
        auto wrapper = [&](const std::filesystem::path & p)
        {
            if(p.is_absolute())
                return p;
            else
                return build_dir/p;
        };
        
        const structure::Recipe *active_recipe = nullptr;
        for (auto * recipe: recipes)
        {
            MSS(write_recipe(*recipe, n, recipes, true, wrapper));
            if (recipe->uri() == uri)
                active_recipe = recipe;
        }

        {
            auto nn = n.open("active");
            MSS(!!active_recipe);
            nn.attr("uri", active_recipe->uri());
        }
        
        MSS_END();
    }
    

} }

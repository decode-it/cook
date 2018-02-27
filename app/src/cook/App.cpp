#include "cook/App.hpp"
#include "cook/chai/Kitchen.hpp"
#include "cook/model/Menu.hpp"
#include "cook/chef/CompileLinkArchive.hpp"
#include "cook/algo/Book.hpp"
#include "gubg/mss.hpp"
#include <unordered_set>

namespace  { 
    const char *logns = nullptr;
} 

namespace cook {

bool App::initialize(const app::Options & options)
{
    MSS_BEGIN(bool);
    MSS(options.valid());

    options_ = options;
    MSS_END();
}

Result App::process()
{
    MSS_BEGIN(Result, logns);

    if (options_.verbosity >= 2)
        options_.stream(std::cout, indent_);

    // initialize the kitchen
    MSS(kitchen_.initialize());

    // set all the variables
    for(const auto & v : options_.variables)
        MSS(kitchen_.register_variable(v.first, v.second));

    // process all files
    MSS(load_recipes_());

    // extract all root recipes
    std::list<model::Recipe*> root_recipes;
    MSS(extract_root_recipes_(root_recipes));

    // and construct a dependency graph
    model::Menu g(kitchen_.root_book());
    MSS(g.construct(gubg::make_range(root_recipes)));

    // process all the requested visualizations
    for(const auto & p: options_.visualizers)
    {
        Kitchen::VisualizerPtr ptr = kitchen_.get_visualizer(p.first);
        MSG_MSS(!!ptr, Error, "unknown visualizer '" << p.first << "'");

        MSS(ptr->set_option(p.second));
        MSS(ptr->process(g, *kitchen_.environment()));
    }

   MSG_MSS(g.all_dependencies_resolved(), Error, "The requested recipes contain unresolved dependencies");
   MSG_MSS(g.is_acyclic(), Error, "The requested recipes have cyclic dependencies");

   {
       chef::LinkArchiveChef lac("default");
       MSS(lac.initialize());

       MSS(lac.mis_en_place(kitchen_, g));
   }

    // resolve the root book
    MSS_END();
}

Result App::load_recipes_()
{
    MSS_BEGIN(Result);

    if (options_.recipe_files.empty())
        options_.recipe_files.push_back("./");

    for(const auto & fn : options_.recipe_files)
        MSS(kitchen_.load(fn));

    MSS_END();
}

Result App::extract_root_recipes_(std::list<model::Recipe*> & result) const
{
    MSS_BEGIN(Result);

    if (options_.recipes.empty())
    {
        cook::algo::list_all_recipes(kitchen_.root_book(), result);
        MSS_RETURN_OK();
    }
    else
    {
        // try to find all recipes
        Result rc;
        for(const auto & v : options_.recipes)
        {
            model::Recipe * recipe = nullptr;
            rc.merge(kitchen_.find_recipe(recipe, v));

            if (recipe)
                result.push_back(recipe);
        }
        MSS(rc);
    }

    MSS_END();
}

}


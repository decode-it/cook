#include "cook/App.hpp"
#include "cook/chai/Kitchen.hpp"
#include "cook/algo/Book.hpp"
#include "cook/algo/Recipe.hpp"
#include "gubg/mss.hpp"
#include <unordered_set>

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
    MSS_BEGIN(Result);

    // initialize the kitchen
    MSS(kitchen_.initialize());

    // set all the variables
    for(const auto & v : options_.variables)
        MSS(kitchen_.register_variable(v.first, v.second));

    // process all files
    MSS(kitchen_.load(options_.recipe_files));

    // try to extract all root recipes
    std::list<model::Recipe*> root_recipes;
    MSS(extract_root_recipes_(root_recipes));

    // propagate the dependencies as far as possible
    bool all_resolved = true;
    {
        MSS(propagate_dependencies_(root_recipes, all_resolved));

        // do the visualizing


        MSG_MSS(all_resolved, Error, "Unable to resolve all recipes");
    }

    // resolve the root book
    MSS_END();
}

Result App::propagate_dependencies_(const std::list<model::Recipe*> & root_recipes, bool & all_resolved) const
{
    MSS_BEGIN(Result);

    all_resolved = true;


    std::stack<model::Recipe*> todo;
    for(model::Recipe * recipe : root_recipes)
        todo.push(recipe);

    std::unordered_set<model::Recipe*> seen;
    while(!todo.empty())
    {
        model::Recipe * recipe = todo.top();
        todo.pop();

        if (!seen.insert(recipe).second)
            continue;

        MSS(algo::resolve_dependencies(recipe, &all_resolved));
    }

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


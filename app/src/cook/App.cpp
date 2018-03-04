#include "cook/App.hpp"
#include "cook/process/chef/CompileLinkArchive.hpp"
#include "cook/algo/Book.hpp"
#include "cook/util/File.hpp"
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
        MSS(kitchen_.set_variable(v.first, v.second));

    // process all files
    MSS(load_recipes_());

    // extract all root recipes
    std::list<model::Recipe*> root_recipes;
    MSS(extract_root_recipes_(root_recipes));

    // and construct the menu
    {
        Result rc = kitchen_.initialize_menu(root_recipes);

        if (!rc)
            MSS(process_generators_());

        MSS(rc);
    }


    // process the menu with the chef
    {
        process::chef::LinkArchiveChef lac("default");
        MSS(lac.initialize());

        MSS(lac.mis_en_place(kitchen_));
    }

    // and now process all the requested visualizations
    MSS(process_generators_());

    // resolve the root book
    MSS_END();
}

Result App::process_generator_(const std::string & name, const std::string & value) const
{
    MSS_BEGIN(Result);

    Context::GeneratorPtr ptr = kitchen_.get_generator(name);
    MSG_MSS(!!ptr, Error, "unknown visualizer '" << name << "'");

    MSS(ptr->set_option(value));

    if (ptr->can_process(kitchen_))
    {
        std::ofstream ofs;
        std::filesystem::path path = ptr->output_filename(kitchen_.dirs());
        MSS(util::open_file(path, ofs));
        MSS(ptr->process(ofs, kitchen_));
    }

    MSS_END();
}


Result App::process_generators_() const
{
    MSS_BEGIN(Result);

    Result rc;
    for(const auto & p: options_.generators)
        rc.merge(process_generator_(p.first, p.second));

    MSS(rc);

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
        result = kitchen_.lib().list_all_recipes();
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


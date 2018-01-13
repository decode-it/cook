#include "cook/Presenter.hpp"
#include "cook/presenter/NinjaWriter.hpp"
#include "cook/presenter/NaftWriter.hpp"
#include "gubg/Strange.hpp"

namespace cook { 

    bool Presenter::set(const std::string &p_key, const std::string &value)
    {
        MSS_BEGIN(bool);
        view_.log(Info) << "Received message: " << p_key << std::endl;

        gubg::Strange key(p_key);

        if (false) {}
        else if (key.pop_if("script.filename")) { script_fn_ = value; }
        else if (key.pop_if("help.message")) { model_.help_message = value; }
        else if (key.pop_if("env.dir.build")) { model_.env.set_dir("build", value); }
        else if (key.pop_if("env.dir.output")) { model_.env.set_dir("output", value); }
        else if (key.pop_if("toolchain.name")) { model_.toolchain.set_name(value); }
        else if (key.pop_if("toolchain.config")) { model_.toolchain.set_config(value); }
        else if (key.pop_if("toolchain.arch")) { model_.toolchain.set_arch(value); }
        else if (key.pop_if("model."))
        {
            if (key.pop_if("book."))
            {
                if (key.pop_if("create"))
                {
                    auto book = model_.library.goc_book(value);
                    MSS(!!book, view_.log(Error) << "Could not goc book " << value << std::endl);
                    MSS(book->set("script_filename", script_fn_), view_.log(Error) << "Failed to set the script filename" << std::endl); 
                }
                else {MSS(false, view_.log(Error) << "Unknown operation " << key << " on book" << std::endl);}
            }
            else
            {
                MSS(false, view_.log(Error) << "Unknown model target " << key << std::endl);
            }
        }
        else if (key.pop_if("action."))
        {
            if (key.pop_if("generate."))
            {
                if (key.pop_if("ninja"))
                {
                    model::RecipeDAG dag;
                    const auto rn = value;
                    MSS(model_.library.get(dag, rn), view_.log(Error) << "Could not extract the DAG for " << rn << std::endl);
                    {
                        std::ofstream fo(model_.env.dir("output") / "build.ninja");
                        presenter::NinjaWriter nw(fo);
                        MSS(nw.write(model_.env, model_.toolchain, dag));
                    }
                }
                else if (key.pop_if("details"))
                {
                    model::RecipeDAG dag;
                    const auto rn = value;
                    MSS(model_.library.get(dag, rn), view_.log(Error) << "Could not extract the DAG for " << rn << std::endl);
                    {
                        presenter::NaftWriter nw(std::cout);
                        MSS(nw.write_details(dag, model_.env.dir("build")));
                    }
                }
                else if (key.pop_if("structure"))
                {
                    model::RecipeDAG dag;
                    //We currently do not use the dag when writing the structure, but we still need to execute this
                    //to make sure all recipes are propely merged according to the dependency structure.
                    MSS(model_.library.get(dag, ""), view_.log(Error) << "Could not extract the DAG" << std::endl);
                    presenter::NaftWriter nw(std::cout);
                    MSS(nw.write_structure(model_.library));
                }
            }
            else
            {
                MSS(false, view_.log(Error) << "Unknown action " << key << std::endl);
            }
        }
        else
        {
            MSS(false, view_.log(Error) << "Unknown message key " << key << std::endl);
        }
        MSS_END();
    }

    bool Presenter::set(const std::string &p_key, const Strings &args)
    {
        MSS_BEGIN(bool);
        view_.log(Info) << "Received message: " << p_key << std::endl;

        gubg::Strange key(p_key);

        if (key.pop_if("model."))
        {
            if (key.pop_if("book."))
            {
                MSS(args.size() >= 1, view_.log(Error) << "At least the uri should be given" << std::endl);
                const auto &uri = args[0];
                auto book = model_.library.goc_book(uri);
                MSS(!!book, view_.log(Error) << "Book " << uri << " does not exists or could not be created" << std::endl); 
                if (false) {}
                else if (key.pop_if("display_name"))
                {
                    MSS(args.size() >= 2, view_.log(Error) << "Not enough arguments for setting the display name" << std::endl);
                    const auto dn = args[1];
                    MSS(book->set("display_name", dn), view_.log(Error) << "Failed to set the display name to " << dn << std::endl); 
                }
                else { MSS(false, view_.log(Error) << "Unknown operation " << key << " on book" << std::endl); }
            }
            else if (key.pop_if("recipe."))
            {
                if (key.pop_if("create"))
                {
                    MSS(args.size() >= 3, view_.log(Error) << "Not enough arguments for creating a recipe" << std::endl);
                    const auto &uri = args[0];
                    auto recipe = model_.library.create_recipe(uri);
                    MSS(!!recipe, view_.log(Error) << "Recipe " << uri << " already exists" << std::endl); 
                    MSS(recipe->set("script_filename", script_fn_), view_.log(Error) << "Failed to set the script filename" << std::endl); 
                    const auto &type = args[1];
                    MSS(recipe->set("type", type), view_.log(Error) << "Unknown type " << type << " already exists" << std::endl); 
                    const auto &wd = args[2];
                    MSS(recipe->set("working_directory", wd), view_.log(Error) << "Failed to set the working directory" << std::endl); 
                    MSS(recipe->set("output_directory", model_.env.dir("output")), view_.log(Error) << "Failed to set the output directory" << std::endl); 
                }
                else
                {
                    MSS(args.size() >= 1, view_.log(Error) << "At least the uri should be given" << std::endl);
                    const auto &uri = args[0];
                    auto recipe = model_.library.get_recipe(uri);
                    MSS(!!recipe, view_.log(Error) << "Recipe " << uri << " does not exists" << std::endl); 
                    if (false) {}
                    else if (key.pop_if("add"))
                    {
                        MSS(args.size() >= 4, view_.log(Error) << "Not enough arguments for adding files to a recipe" << std::endl);
                        recipe->add(args[1], args[2], args[3]);
                    }
                    else if (key.pop_if("add_file"))
                    {
                        MSS(args.size() == 4, view_.log(Error) << "Not enough arguments for adding a file to a recipe" << std::endl);
                        MSS(recipe->add_file(args[1], args[2], args[3]), view_.log(Error) << "Unable to add the file " << args[2] << " to recipe " << args[1] << std::endl);
                    }
                    else if (key.pop_if("depends_on"))
                    {
                        MSS(args.size() >= 2, view_.log(Error) << "Not enough arguments for specifying a dependency" << std::endl);
                        const auto rn = args[1];
                        MSS(recipe->set("depends_on", rn), view_.log(Error) << "Failed to set the dependency on " << rn << std::endl); 
                    }
                    else if (key.pop_if("display_name"))
                    {
                        MSS(args.size() >= 2, view_.log(Error) << "Not enough arguments for setting the display name" << std::endl);
                        const auto dn = args[1];
                        MSS(recipe->set("display_name", dn), view_.log(Error) << "Failed to set the display name to " << dn << std::endl); 
                    }
                    else if (key.pop_if("library_path"))
                    {
                        MSS(args.size() >= 2, view_.log(Error) << "Not enough arguments for adding a library path" << std::endl);
                        const auto name = args[1];
                        MSS(recipe->set("library_path", name), view_.log(Error) << "Failed to add library " << name << std::endl);
                    }
                    else if (key.pop_if("library"))
                    {
                        MSS(args.size() >= 2, view_.log(Error) << "Not enough arguments for adding a library" << std::endl);
                        const auto name = args[1];
                        MSS(recipe->set("library", name), view_.log(Error) << "Failed to add library " << name << std::endl); 
                    }
                    else {MSS(false, view_.log(Error) << "Unknown operation " << key << " on recipe" << std::endl);}
                }
            }
            else
            {
                MSS(false, view_.log(Error) << "Unknown model target " << key << std::endl);
            }
        }
        else
        {
            MSS(false, view_.log(Error) << "Unknown message key " << key << std::endl);
        }
        MSS_END();
    }

    bool Presenter::set(const std::string &p_key)
    {
        MSS_BEGIN(bool);
        view_.log(Info) << "Received message: " << p_key << std::endl;

        gubg::Strange key(p_key);

        if (key.pop_if("runner.done"))
        {
            MSS(model_.library.resolve(), view_.log(Error) << "Failed to resolve the dependencies in the library" << std::endl);
        }
        else if (key.pop_if("action."))
        {
            if (key.pop_if("print."))
            {
                if (false) {}
                else if (key.pop_if("help"))  view_.log(Always) << model_.help_message << std::endl;
                else if (key.pop_if("books")) model_.library.stream(view_.log(Message));
            }
            else
            {
                MSS(false, view_.log(Error) << "Unknown action " << key << std::endl);
            }
        }
        else
        {
            MSS(false, view_.log(Error) << "Unknown message key " << key << std::endl);
        }
        MSS_END();
    }

    bool Presenter::get(const std::string & p_key, std::string & result) const
    {
        MSS_BEGIN(bool);
        view_.log(Info) << "Received request: " << p_key << std::endl;
        gubg::Strange key(p_key);

        if (false) {}
        else { MSS(false, view_.log(Error) << "Unknown request key " << key << std::endl); }

        MSS_END();
    }
    bool Presenter::get(const std::string & p_key, const Strings & args, std::string & result) const
    {
        MSS_BEGIN(bool);
        view_.log(Info) << "Received request: " << p_key << std::endl;
        gubg::Strange key(p_key);

        if (false) {}
        if (key.pop_if("model."))
        {
            if (key.pop_if("recipe."))
            {
                MSS(args.size() >= 1, view_.log(Error) << "At least the uri should be given" << std::endl);
                const auto &uri = args[0];
                auto recipe = model_.library.get_recipe(uri);
                MSS(!!recipe, view_.log(Error) << "Recipe " << uri << " does not exists" << std::endl);
                if (false) {}
                else
                {
                    MSS(args.size() == 1, view_.log(Error) << "To much arguments for requesting " << key.str() << std::endl);
                    MSS(recipe->get(key.str(), result), view_.log(Error) << "Unable to request" << key.str() << std::endl);
                }
            }
            else { MSS(false, view_.log(Error) << "Unknown model target " << key << std::endl); }

        }
        else { MSS(false, view_.log(Error) << "Unknown request key " << key << std::endl); }


        MSS_END();
    }

} 

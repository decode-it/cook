#include "cook/Presenter.hpp"
#include "cook/presenter/NinjaWriter.hpp"
#include "gubg/Strange.hpp"

namespace cook { 

    bool Presenter::set(const std::string &p_key, const std::string &value)
    {
        MSS_BEGIN(bool);
        view_.log(Info) << "Received message: " << p_key << std::endl;

        gubg::Strange key(p_key);

        if (false) {}
        else if (key.pop_if("help.message")) { model_.help_message = value; }
        else if (key.pop_if("env.build_dir")) { model_.env.set_build_dir(value); }
        else if (key.pop_if("toolchain.config")) { model_.toolchain.set_config(value); }
        else if (key.pop_if("toolchain.arch")) { model_.toolchain.set_arch(value); }
        else if (key.pop_if("model."))
        {
            if (false) {}
            else if (key.pop_if("book."))
            {
                if (false) {}
                else if (key.pop_if("push")) { model_.library.push(value); }
                else if (key.pop_if("pop")) { model_.library.pop(); }
                else {MSS(false, view_.log(Error) << "Unknown operation " << key << " on book");}
            }
            else if (key.pop_if("recipe."))
            {
                if (false) {}
                else if (key.pop_if("create"))
                {
                    const auto name = value;
                    MSS(model_.library.create_recipe(name), view_.log(Error) << "Recipe " << name << " already exists" << std::endl); 
                }
                else if (key.pop_if("close")) { model_.library.close_recipe(); }
                else
                {
                    auto recipe = model_.library.current_recipe();
                    MSS(!!recipe, view_.log(Error) << "No current recipe" << std::endl);
                    if (false) {}
                    else if (key.pop_if("type"))
                    {
                        const auto type = value;
                        MSS(recipe->set("type", type), view_.log(Error) << "Unknown type " << type << " already exists" << std::endl); 
                    }
                    else if (key.pop_if("working_directory"))
                    {
                        MSS(recipe->set("working_directory", value), view_.log(Error) << "Failed to set the working directory" << std::endl); 
                    }
                    else if (key.pop_if("depends_on"))
                    {
                        const auto rn = value;
                        MSS(recipe->set("depends_on", rn), view_.log(Error) << "Failed to set the dependency on " << rn << std::endl); 
                    }
                    else {MSS(false, view_.log(Error) << "Unknown operation " << key << " on recipe");}
                }
            }
            else
            {
                MSS(false, view_.log(Error) << "Unknown model target " << key << std::endl);
            }
        }
        else if (key.pop_if("action."))
        {
            if (false) {}
            else if (key.pop_if("print."))
            {
                if (false) {}
                else if (key.pop_if("help"))
                    view_.log(Message) << model_.help_message << std::endl;
                else if (key.pop_if("books"))
                    model_.library.stream(view_.log(Message));
            }
            else if (key.pop_if("generate."))
            {
                if (false) {}
                else if (key.pop_if("ninja"))
                {
                    model::RecipeDAG dag;
                    const auto rn = value;
                    MSS(model_.library.get(dag, rn), view_.log(Error) << "Could not extract the DAG for " << rn << std::endl);
                    dag.each_vertex<gubg::network::Direction::Forward>([&](auto r){ view_.log(Info) << *r << std::endl; return true; });
                    presenter::NinjaWriter nw("test.ninja");
                    MSS(nw.write(model_.env, model_.toolchain, dag));
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

    bool Presenter::set(const std::string &p_key, const Strings &value)
    {
        MSS_BEGIN(bool);
        view_.log(Info) << "Received message: " << p_key << std::endl;

        gubg::Strange key(p_key);

        if (false) {}
        else if (key.pop_if("model."))
        {
            if (false) {}
            else if (key.pop_if("recipe."))
            {
                if (false) {}
                else
                {
                    auto recipe = model_.library.current_recipe();
                    MSS(!!recipe, view_.log(Error) << "No current recipe" << std::endl);
                    if (false) {}
                    else if (key.pop_if("add"))
                    {
                        const auto &args = value;
                        MSS(args.size() >= 2, view_.log(Error) << "Not enough arguments for adding files to a recipe" << std::endl);
                        recipe->add(args[0], args[1]);
                    }
                    else {MSS(false, view_.log(Error) << "Unknown operation " << key << " on recipe");}
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

        if (false) {}
        else if (key.pop_if("action."))
        {
            if (false) {}
            else if (key.pop_if("print."))
            {
                if (false) {}
                else if (key.pop_if("help"))
                    view_.log(Message) << model_.help_message << std::endl;
                else if (key.pop_if("books"))
                    model_.library.stream(view_.log(Message));
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

} 

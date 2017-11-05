#include "cook/Presenter.hpp"
#include "gubg/Strange.hpp"

namespace cook { 

    bool Presenter::set(const std::string &p_key, const std::any &value)
    {
        MSS_BEGIN(bool);
        view_.log(Info) << "Received message: " << p_key << std::endl;

        gubg::Strange key(p_key);

        if (false) {}
        else if (key.pop_if("help.message")) { model_.help_message = std::any_cast<std::string>(value); }
        else if (key.pop_if("toolchain.config")) { model_.toolchain.set_config(std::any_cast<std::string>(value)); }
        else if (key.pop_if("toolchain.arch")) { model_.toolchain.set_arch(std::any_cast<std::string>(value)); }
        else if (key.pop_if("model."))
        {
            if (false) {}
            else if (key.pop_if("book."))
            {
                if (false) {}
                else if (key.pop_if("push")) { model_.library.push(std::any_cast<std::string>(value)); }
                else if (key.pop_if("pop")) { model_.library.pop(); }
                else {MSS(false, view_.log(Error) << "Unknown operation " << key << " on book");}
            }
            else if (key.pop_if("recipe."))
            {
                if (false) {}
                else if (key.pop_if("create"))
                {
                    const auto name = std::any_cast<std::string>(value);
                    MSS(model_.library.create_recipe(name), view_.log(Error) << "Recipe " << name << " already exists" << std::endl); 
                }
                else if (key.pop_if("close")) { model_.library.close_recipe(); }
                else
                {
                    auto recipe = model_.library.recipe();
                    MSS(!!recipe, view_.log(Error) << "No current recipe" << std::endl);
                    if (false) {}
                    else if (key.pop_if("type"))
                    {
                        const auto type = std::any_cast<std::string>(value);
                        MSS(recipe->set("type", type), view_.log(Error) << "Unknown type " << type << " already exists" << std::endl); 
                    }
                    else if (key.pop_if("working_directory"))
                    {
                        MSS(recipe->set("working_directory", std::any_cast<std::string>(value)), view_.log(Error) << "Failed to set the working directory" << std::endl); 
                    }
                    else if (key.pop_if("add"))
                    {
                        const auto &args = std::any_cast<const Strings &>(value);
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

#include "cook/Presenter.hpp"

namespace cook { 

    bool Presenter::receive_(const std::string &key, const std::any &value)
    {
        MSS_BEGIN(bool);
        view_.log(Info) << "Received message: " << key << std::endl;

        if (false) {}
        else if (key == "help.message") { model_.help_message = std::any_cast<std::string>(value); }
        else if (key == "toolchain.config") { model_.toolchain.set_config(std::any_cast<std::string>(value)); }
        else if (key == "toolchain.arch") { model_.toolchain.set_arch(std::any_cast<std::string>(value)); }
        else if (key == "action")
        {
            const auto action = std::any_cast<std::string>(value);
            if (false) {}
            else if (action == "print_help")
            {
                view_.log(Message) << model_.help_message << std::endl;
            }
            else
            {
                MSS(false, view_.log(Error) << "Unknown action " << action << std::endl);
            }
        }
        else
        {
            MSS(false, view_.log(Error) << "Unknown message key " << key << std::endl);
        }
        MSS_END();
    }

} 

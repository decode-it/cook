#include "cook/Presenter.hpp"

namespace cook { 

    void Presenter::receive_(const std::string &key, const std::any &value)
    {
        view_.log() << "Received message: " << key << std::endl;

        if (false) {}
        else if (key == "help.message")
        {
            model_.help_message = std::any_cast<std::string>(value);
            view_.log() << model_.help_message << std::endl;
        }
    }

} 

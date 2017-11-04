#ifndef HEADER_cook_View_hpp_ALREADY_INCLUDED
#define HEADER_cook_View_hpp_ALREADY_INCLUDED

#include "cook/Types.hpp"
#include "gubg/mss.hpp"
#include <iostream>
#include <functional>
#include <string>
#include <any>

namespace cook { 

    class View
    {
    public:
        using Send_func = std::function<bool(const std::string &/*key*/, const std::any &/*value*/)>;

        void set_receiver(const Send_func &receiver) {send_ = receiver;}

        //Message to the presenter
        bool send(const std::string &key, const std::any &value);

        std::ostream &log(LogType lt = Message);

        bool process(int argc, const char **argv);

    private:
        Send_func send_;
    };

} 

#endif

#ifndef HEADER_cook_View_hpp_ALREADY_INCLUDED
#define HEADER_cook_View_hpp_ALREADY_INCLUDED

#include <iostream>
#include <functional>
#include <string>
#include <any>

namespace cook { 

    class View
    {
    public:
        //Message to the presenter
        std::function<void(const std::string &/*key*/, const std::any &/*value*/)> send;

        std::ostream &log() {return std::cout;}

        void process(int argc, const char **argv);

    private:
    };

} 

#endif

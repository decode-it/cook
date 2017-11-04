#ifndef HEADER_cook_View_hpp_ALREADY_INCLUDED
#define HEADER_cook_View_hpp_ALREADY_INCLUDED

#include <iostream>

namespace cook { 

    class View
    {
    public:
        std::ostream &log() {return std::cout;}

    private:
    };

} 

#endif

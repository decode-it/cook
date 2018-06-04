#ifndef HEADER_cook_Hook_hpp_ALREADY_INCLUDED
#define HEADER_cook_Hook_hpp_ALREADY_INCLUDED

#include <ostream>

namespace cook {

    enum class Hook
    {
        Selected = 0,
        Pre,
        Post,
        _End
    };

    std::ostream & operator<<(std::ostream & str, Hook hook);

}

#endif


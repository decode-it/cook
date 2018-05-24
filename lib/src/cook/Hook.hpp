#ifndef HEADER_cook_Hook_hpp_ALREADY_INCLUDED
#define HEADER_cook_Hook_hpp_ALREADY_INCLUDED

#include <ostream>

namespace cook {

    enum class Hook
    {
        Pre = 0,
        Post,
        _End
    };

    std::ostream & operator<<(std::ostream & str, Hook hook);

}

#endif


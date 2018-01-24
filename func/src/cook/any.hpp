#ifndef HEADER_cook_any_hpp_ALREADY_INCLUDED
#define HEADER_cook_any_hpp_ALREADY_INCLUDED

#include <any>
#include <vector>

namespace cook {

template <typename ... Args> std::vector<std::any> as_any(const Args ... args)
{
    return std::vector<std::any>( { std::any(args)... } );
}

}

#endif

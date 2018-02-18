#ifndef HEADER_cook_Propagation_hpp_ALREADY_INCLUDED
#define HEADER_cook_Propagation_hpp_ALREADY_INCLUDED

#include <ostream>

namespace cook {

enum class Propagation
{
    Private = 0,
    Public = 1,
};

inline std::ostream & operator<<(std::ostream & os, Propagation propagation)
{

    switch(propagation)
    {
#define L_CASE(NAME) case Propagation::NAME: os << #NAME; return os
        L_CASE(Private);
        L_CASE(Public);
#undef L_CASE
        default:
            os << "unknown";
    }
    return os;
}

}

#endif

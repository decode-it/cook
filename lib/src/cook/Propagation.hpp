#ifndef HEADER_cook_Propagation_hpp_ALREADY_INCLUDED
#define HEADER_cook_Propagation_hpp_ALREADY_INCLUDED

#include <ostream>

namespace cook {

enum class Propagation
{
    Private,
    Public,
};

inline std::ostream & operator<<(std::ostream & str, Propagation propagation)
{

    switch(propagation)
    {
#define L_CASE(NAME) case Propagation::NAME: str << #NAME; return str
        L_CASE(Private);
        L_CASE(Public);
#undef L_CASE
        default:
            str << "unknown";
    }
    return str;
}

}

#endif

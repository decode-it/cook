#ifndef HEADER_cook_Overwrite_hpp_ALREADY_INCLUDED
#define HEADER_cook_Overwrite_hpp_ALREADY_INCLUDED

#include <ostream>

namespace cook {

enum class Overwrite
{
    Never = 0x00,
    IfSame = 0x01,
    Always = 0x02,
};

inline bool merge(Overwrite & old_value, Overwrite new_value)
{
    // by default take the most strict
    old_value = std::min(old_value, new_value);
    return true;
}

inline std::ostream & operator<<(std::ostream & os, Overwrite overwrite)
{

    switch(overwrite)
    {
#define L_CASE(NAME) case Overwrite::NAME: os << #NAME; return os
        L_CASE(Never);
        L_CASE(IfSame);
        L_CASE(Always);
#undef L_CASE

        default:
            os << "unknown";
    }


    return os;
}

}

#endif

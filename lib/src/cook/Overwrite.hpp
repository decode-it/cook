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

inline std::ostream & operator<<(std::ostream & str, Overwrite overwrite)
{

    switch(overwrite)
    {
#define L_CASE(NAME) case Overwrite::NAME: str << #NAME; return str
        L_CASE(Never);
        L_CASE(IfSame);
        L_CASE(Always);
#undef L_CASE

        default:
            str << "unknown";
    }


    return str;
}

}

#endif
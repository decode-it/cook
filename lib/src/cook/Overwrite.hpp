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
#define L_CASE(NAME, VALUE) case Overwrite::NAME: str << "overwrite=" << VALUE; return str;
        L_CASE(Never, "never");
        L_CASE(IfSame, "if_same");
        L_CASE(Always, "always");
#undef L_CASE

        default:
            str << "overwrite=unknown";
    }


    return str;
}

}

#endif

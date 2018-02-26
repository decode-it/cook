#ifndef HEADER_cook_Language_hpp_ALREADY_INCLUDED
#define HEADER_cook_Language_hpp_ALREADY_INCLUDED

#include <ostream>

namespace cook {

enum class Language
{
    Undefined,
    Binary,
    C,
    CXX,
    ASM,
    UserDefined
};

inline std::ostream & operator<<(std::ostream & os, Language type)
{
    switch(type)
    {
#define L_CASE(NAME) case Language::NAME: os << #NAME; return os
        L_CASE(Undefined);
        L_CASE(Binary);
        L_CASE(C);
        L_CASE(CXX);
        L_CASE(ASM);
#undef L_CASE
        default:
            break;
    }

    if (type >= Language::UserDefined)
        os << "UserDefined (" << static_cast<unsigned int>(type) << ")";
    else
        os << "unknown";

    return os;
}

}

#endif

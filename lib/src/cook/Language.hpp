#ifndef HEADER_cook_Language_hpp_ALREADY_INCLUDED
#define HEADER_cook_Language_hpp_ALREADY_INCLUDED

#include <ostream>

namespace cook {

enum class Language
{
    Undefined,
    C,
    CXX,
    ASM,
    UserDefined
};

inline std::ostream & operator<<(std::ostream & str, Language type)
{
    switch(type)
    {
#define L_CASE(NAME) case Language::NAME: str << #NAME; return str
        L_CASE(Undefined);
        L_CASE(C);
        L_CASE(CXX);
        L_CASE(ASM);
#undef L_CASE
        default:
            break;
    }

    if (type >= Language::UserDefined)
        str << "UserDefined (" << static_cast<unsigned int>(type) << ")";
    else
        str << "unknown";

    return str;
}

}

#endif

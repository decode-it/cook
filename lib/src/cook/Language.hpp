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
    Script,
    UserDefined
};

inline std::ostream & operator<<(std::ostream & os, Language type)
{
    switch(type)
    {
#define L_CASE(NAME) case Language::NAME: return os << #NAME
        L_CASE(Undefined);
        L_CASE(Binary);
        L_CASE(C);
        L_CASE(CXX);
        L_CASE(ASM);
        L_CASE(Script);
#undef L_CASE
        default:
        return os << "UserDefined(" << (static_cast<unsigned int>(type) - static_cast<unsigned int>(Language::UserDefined)) << ")";
    }
}

}

#endif

#ifndef HEADER_cook_Flags_hpp_ALREADY_INCLUDED
#define HEADER_cook_Flags_hpp_ALREADY_INCLUDED

#include "cook/Flags.h"
#include <ostream>

#define COOK_FLAG_OPERATORS(TYPE) \
    inline TYPE operator |(TYPE lhs, TYPE rhs) \
{ \
    return static_cast<TYPE>(static_cast<int>(lhs) | static_cast<int>(rhs)); \
    } \
    inline TYPE & operator |=(TYPE & lhs, TYPE rhs) \
{ \
    lhs = lhs | rhs; \
    return lhs; \
    }

template <typename T>
bool is_set(T lhs, T rhs) { return (lhs&rhs) == rhs; }

inline std::ostream &  operator<<(std::ostream & str, cook_Flags_t type)
{
    unsigned int cnt = 0;

    #define CHECK(VAL) if (is_set(type, cook_Flags_ ## VAL)) str << (cnt++ > 0 ? "|" : "") << #VAL
    CHECK(Overwrite_Never);
    CHECK(Overwrite_IfSame);
    CHECK(Overwrite_Always);
    CHECK(Propagation_Public);
    CHECK(Propagation_Private);
    CHECK(ForceInclude);
    #undef CHECK


    return str;
}

COOK_FLAG_OPERATORS(cook_Flags_t)

#endif

#ifndef HEADER_cook_chai_Flags_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Flags_hpp_ALREADY_INCLUDED

#include "cook/Language.hpp"
#include "cook/Type.hpp"
#include "cook/TargetType.hpp"
#include "cook/Overwrite.hpp"
#include "cook/Propagation.hpp"
#include "cook/chai/mss.hpp"

namespace cook { namespace chai {

enum class Flag
{
    Language,
    Type,
    TargetType,
    Overwrite,
    Propagation,
    __END
};

std::ostream & operator<<(std::ostream & str, Flag flag);

class Flags
{
public:


    Flags() : flags_(0) { }

#define CONSTRUCTOR(TYPE) Flags(TYPE val) { set(val); }
    CONSTRUCTOR(Language)
    CONSTRUCTOR(Type)
    CONSTRUCTOR(TargetType)
    CONSTRUCTOR(Overwrite)
    CONSTRUCTOR(Propagation)
#undef CONSTRUCTOR

    Flags & operator|(const Flags & rhs);
    bool is_set(Flag flag) const;

#define SET(TYPE) void set(TYPE value) { flags_ |= (1u << static_cast<int>(Flag::TYPE)); t##TYPE##_ = value; }
    SET(Language)
    SET(Type)
    SET(TargetType)
    SET(Overwrite)
    SET(Propagation)
#undef SET

#define GET_IF(TYPE) TYPE get_or(TYPE fallback) const { return is_set(Flag::TYPE) ? t##TYPE##_ : fallback; }
    GET_IF(Language)
    GET_IF(Type)
    GET_IF(TargetType)
    GET_IF(Overwrite)
    GET_IF(Propagation)
#undef GET_IF

    bool only(const std::initializer_list<Flag> & lst) const;

private:
    unsigned int flags_;
    Language tLanguage_;
    Type tType_;
    TargetType tTargetType_;
    Overwrite tOverwrite_;
    Propagation tPropagation_;
};


} }

#endif


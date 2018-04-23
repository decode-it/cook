#include "cook/chai/Flags.hpp"
#include "cook/log/Scope.hpp"

namespace cook { namespace chai {

std::ostream & operator<<(std::ostream & str, Flag flag)
{
    switch(flag)
    {
#define L_CASE(TYPE) case Flag::TYPE: return str << #TYPE
    L_CASE(Language);
    L_CASE(Type);
    L_CASE(TargetType);
    L_CASE(Overwrite);
    L_CASE(Propagation);
    default:
        return str << "<unknown>";
    }
}

Flags::Flags()
    : flags_(0)
{

}

Result Flags::only(const std::initializer_list<Flag> & lst) const
{
    MSS_BEGIN(Result);

    auto s = cook::log::scope("Check flags", [&](auto & n) {
        for(Flag f : lst)
            n.attr("allowed", f);
    });

    unsigned int flags = flags_;

    for(auto fl : lst)
        flags &= (~(1u << static_cast<unsigned int>(fl)));

    for(int i = 0; i < static_cast<int>(Flag::_End); ++i)
        MSG_MSS((flags & (1u<<i)) == 0, Error, "Flag " << static_cast<Flag>(i) << " should not be set");

    MSG_MSS(flags == 0, Error, "Unknown flag has been set");

    MSS_END();
}

Flags & Flags::operator|(const Flags & rhs)
{
    CHAI_MSS_BEGIN();

#define SET_IF(NAME) if(rhs.is_set(Flag::NAME)) { CHAI_MSS_MSG(!is_set(Flag::NAME) , Error, "Flag " << #NAME << " is set in both values"); set(rhs.t##NAME##_); }
    SET_IF(Language)
    SET_IF(Type);
    SET_IF(TargetType);
    SET_IF(Overwrite);
    SET_IF(Propagation);
#undef SET_IF

    return *this;
}

bool Flags::is_set(Flag flag) const
{
    return flags_ & (1u << static_cast<unsigned int>(flag));
}

} }



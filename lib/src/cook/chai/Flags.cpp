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
    SET_IF(Overwrite);
    SET_IF(Propagation);
#undef SET_IF

    return *this;
}

bool Flags::is_set(Flag flag) const
{
    return flags_ & (1u << static_cast<unsigned int>(flag));
}

Flags::operator bool() const
{
    return flags_ > 0;
}


namespace  {

template <typename T>
std::pair<T, bool> combine(const std::pair<T, bool> & lhs, const std::pair<T, bool> & rhs)
{
    if (lhs.second && rhs.second && (lhs.first == rhs.first))
        return lhs;
    return std::make_pair(T(), false);
}

template <typename T>
bool equal(const std::pair<T, bool> & lhs, const std::pair<T, bool> & rhs)
{
    if (lhs.second && rhs.second && (lhs.first == rhs.first))
        return true;
    if (!lhs.second && !rhs.second)
        return true;

    return false;
}

}


Flags Flags::operator&(const Flags & rhs) const
{
    Flags result;

#define SET_IF(NAME) { auto p = combine(NAME(), rhs.NAME()); if (p.second) result.set(p.first); }
    SET_IF(language)
    SET_IF(type);
    SET_IF(overwrite);
    SET_IF(propagation);
#undef SET_IF

    return result;
}

bool Flags::operator==(const Flags & rhs) const
{
#define CHECK(NAME) equal(NAME(), rhs.NAME())
    return flags_ == rhs.flags_
            && CHECK(language)
            && CHECK(type)
            && CHECK(overwrite)
            && CHECK(propagation);
}

bool Flags::operator!=(const Flags & rhs) const
{
    return !operator ==(rhs);
}

std::string Flags::to_string() const
{
    std::ostringstream oss;
    bool started = false;
#define GET_IF(NAME) { \
    auto p = NAME(); \
    if (p.second) { \
        if(started) \
            oss << " | "; \
    oss << p.first; started = true; } }
    GET_IF(language)
    GET_IF(type);
    GET_IF(overwrite);
    GET_IF(propagation);
#undef GET_IF

    return oss.str();
}

} }



#ifndef HEADER_cook_Result_hpp_ALREADY_INCLUDED
#define HEADER_cook_Result_hpp_ALREADY_INCLUDED

namespace cook {

namespace result {

enum Type
{
    Success,
    Warning,
    Error,
};

enum Category
{
    None,
    Unknown
};

}

struct Result
{
    Result()
        : type_(result::Success),
          category_(result::None),
          value_(0)
    {
    }

    template <typename Src> Result(result::Type type, result::Category category, Src value)
        : type_(type),
          category_(category),
          value_(static_cast<int>(value))
    {
    }

    operator bool() const
    {
        switch(type_)
        {
            case result::Success:
            case result::Warning:
                return true;
            default:
                return false;
        }
    }

    bool operator==(const Result & rhs) const
    {
        return type_ == rhs.type_
                && category_ == rhs.category_
                && value_ == rhs.value_;
    }

    result::Type type() const { return type_; }
    result::Category category() const { return category_; }
    template <typename Src> Src value() const { return static_cast<Src>(value_); }

private:
    result::Type type_;
    result::Category category_;
    int value_;
};

}


#include "gubg/mss.hpp"

namespace gubg { namespace mss {

// translation from result to bool
template <> bool is_ok(cook::Result c) { return c; }

namespace detail {

// translating a bool to result
template <typename Src> struct ErrorValue<cook::Result, Src>
{
    static cook::Result error_value(Src src){return cook::Result(cook::result::Error, cook::result::Unknown, src); }
};

template <> struct ErrorValue<cook::Result, cook::Result>
{
    static cook::Result error_value(cook::Result src) { return src; }
};

}

template <> cook::Result ok_value<cook::Result>() { return cook::Result(); }

} }


#endif

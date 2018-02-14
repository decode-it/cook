#ifndef HEADER_cook_Result_hpp_ALREADY_INCLUDED
#define HEADER_cook_Result_hpp_ALREADY_INCLUDED

#include "cook/Message.hpp"
#include <list>

namespace cook {

struct Result
{
    Result()
        : type_(MessageType::Undefined)
    {
    }

    bool test_flag(MessageType flag) const
    {
        return (type_ & flag) == flag;
    }

    operator bool() const
    {
        const MessageType succeed_flags = MessageType::Success | MessageType::Warning;
        return (type_ & (~succeed_flags)) == MessageType{};
    }

    Result & operator<<(const Message & message)
    {
        type_ |= message.type;
        messages_.push_back(message);
        return *this;
    }

    template <typename Fctr>
    void each_message(Fctr && fctr) const
    {
        for(const auto & message : messages_)
            fctr(message.type, message.reporter);
    }

private:
    MessageType type_;
    std::list<Message> messages_;
};

}


#include "gubg/mss.hpp"

namespace gubg { namespace mss {

// translation from result to bool
template <> bool is_ok(cook::Result c) { return c; }

namespace detail {

// translating an error of type Src to Result
template <typename Src> struct ErrorValue<cook::Result, Src>
{
    static cook::Result error_value(Src src)
    {
        return cook::Result() << cook::Message(cook::MessageType::Error, [=](auto & os) { os << "Unknown error value: " << src; });
    }
};

template <> struct ErrorValue<cook::Result, cook::Result>
{
    static cook::Result error_value(cook::Result src) { return src; }
};

}

template <> cook::Result ok_value<cook::Result>()
{
    return cook::Result() << cook::Message(cook::MessageType::Success);
}

} }


#endif

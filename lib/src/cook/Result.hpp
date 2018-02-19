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
            const MessageType succeed_flags = MessageType::Success | MessageType::Warning | MessageType::Info;
            return (type_ & (~succeed_flags)) == MessageType{};
        }

        Result & operator<<(const Message & message)
        {
            type_ |= message.type;
            messages_.push_back(message);
            return *this;
        }
        Result & splice(Result & result)
        {
            type_ |= result.type_;
            messages_.splice(messages_.end(), result.messages_);
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
    template <> inline bool is_ok(cook::Result c) { return c; }

    template <> inline cook::Result ok_value<cook::Result>()
    {
        return cook::Result() << cook::Message(cook::MessageType::Success);
    }

    inline void aggregate(cook::Result &dst, cook::Result src)
    {
        dst.splice(src);
    }
    template <typename Src>
    void aggregate(cook::Result &dst, Src src)
    {
        if (!is_ok(src))
            dst << cook::Message(cook::MessageType::Error, [=](auto & os) { os << "Unknown error value: " << src; });
    }
    template <typename Dst>
    void aggregate(Dst &dst, const cook::Result &src)
    {
        aggregate(dst, !!src);
    }

} }


#endif

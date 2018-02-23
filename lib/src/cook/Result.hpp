#ifndef HEADER_cook_Result_hpp_ALREADY_INCLUDED
#define HEADER_cook_Result_hpp_ALREADY_INCLUDED

#include "cook/Message.hpp"
#include "gubg/mss.hpp"
#include <list>
#include <type_traits>

namespace cook {

    struct Result
    {
        Result()
        : type_(MessageType::Success)
        {
        }

        bool test_flag(MessageType flag) const
        {
            return (type_ & flag) == flag;
        }

        void set_flag(MessageType flag)
        {
            type_ |= flag;
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

        Result & operator<<(Message && message)
        {
            type_ |= message.type;
            messages_.push_back(std::move(message));
            return *this;
        }

        Result & operator<<(const Result & rhs)
        {
            return merge(rhs);
        }
        Result & operator<<(Result && rhs)
        {
            return merge(std::move(rhs));
        }

        Result & merge(const Result & rhs)
        {
            type_ |= rhs.type_;
            messages_.insert(messages_.end(), rhs.messages_.begin(), rhs.messages_.end());
            return *this;
        }

        Result & merge(Result && result)
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

    namespace mss {

    template <typename MSG> auto msg(MSG && msg, typename std::enable_if<std::is_same<cook::Message, std::decay_t<MSG>>::value>::type * /*dummy*/ = nullptr)
    {
        return [=](Result & res, auto && src)
        {
            if (!gubg::mss::is_ok(src))
                res << msg;
        };
    }

#define MSG_MSS(COND, TYPE, STR) MSS(COND, {}, ::cook::mss::msg(MESSAGE(TYPE, STR)))

    }
}

namespace gubg { namespace mss {

    // translation from result to bool
    template <> inline bool is_ok(cook::Result c) { return c; }

    template <> inline cook::Result ok_value<::cook::Result>()
    {
        return ::cook::Result();
    }

    inline void aggregate(::cook::Result &dst, const ::cook::Result & src)
    {
        dst.merge(src);
    }
    inline void aggregate(::cook::Result &dst, ::cook::Result && src)
    {
        dst.merge(src);
    }
    template <typename Src> void aggregate(::cook::Result &dst, Src src)
    {
        if (!gubg::mss::is_ok(src))
            dst << MESSAGE(InternalError, "Unexpected error '" << src << "'");
    }
} }


#endif

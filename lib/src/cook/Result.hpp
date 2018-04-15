#ifndef HEADER_cook_Result_hpp_ALREADY_INCLUDED
#define HEADER_cook_Result_hpp_ALREADY_INCLUDED

#include "cook/log/Node.hpp"
#include "gubg/mss.hpp"
#include "gubg/stream.hpp"
#include <list>
#include <memory>
#include <type_traits>

namespace cook {

class Message
{
public:
    enum Type { Info = 0, Warning = 1, Error = 2, InternalError = 3 };

    Message(Type type, log::Ptr node, const std::string & msg = std::string())
        : type_(type),
          node_(node),
          msg_(msg)
    {
    }

    explicit Message(Type type, const std::string & msg = std::string())
        : type_(type),
          node_(log::Node::top_ptr()),
          msg_(msg)
    {

    }

    Type type_;
    log::Ptr node_;
    std::string msg_;
};


inline std::ostream & operator<<(std::ostream & oss, Message::Type type)
{
    switch(type)
    {
#define L_CASE(TYPE) case Message::TYPE: return oss << #TYPE
    L_CASE(Info);
    L_CASE(Warning);
    L_CASE(Error);
    L_CASE(InternalError);
#undef L_CASE
    }

    return oss << "<unknown>";
}

#define MESSAGE(TYPE, STREAM) ::cook::Message(::cook::Message::TYPE, gubg::stream([&](auto & os) { os << STREAM; } ))

class Result
{
public:
    Result() : flags_(0){}

    operator bool() const { return !test_flag(Message::Error) && !test_flag(Message::InternalError); }

    bool test_flag(Message::Type t) const
    {
        unsigned int flag = (1u << t);
        return (flags_&flag) == flag;
    }

    Result & operator<<(const Message & rhs)
    {
        results_.push_back(rhs);
        flags_ |= (1u << rhs.type_);

        return *this;
    }

    Result & operator<<(Message && rhs)
    {
        results_.push_back(std::move(rhs));
        flags_ |= (1u << rhs.type_);

        return *this;
    }

    void append(Message::Type type, log::Ptr node, const std::string & msg = std::string())
    {
        results_.emplace_back(type, node, msg);
        flags_ |= (1u << type);
    }

    Result & merge(const Result & rhs)
    {
        results_.insert(results_.end(), rhs.results_.begin(), rhs.results_.end());
        flags_ |= rhs.flags_;

        return *this;
    }

    Result & merge(Result && rhs)
    {
        results_.splice(results_.end(), rhs.results_);
        flags_ |= rhs.flags_;

        return *this;
    }

    template <typename Fctr>
    void each_message(Fctr && fctr) const
    {
        for(const auto & message : results_)
            fctr(message);
    }

private:
    unsigned int flags_;
    std::list<Message> results_;
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

}

#define MSG_MSS(COND, TYPE, STR) MSS(COND, {}, ::cook::mss::msg(MESSAGE(TYPE, STR)))

}

namespace gubg { namespace mss {

    // translation from result to bool
    template <> inline bool is_ok(::cook::Result c) { return c; }

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

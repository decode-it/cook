#ifndef HEADER_cook_Message_hpp_ALREADY_INCLUDED
#define HEADER_cook_Message_hpp_ALREADY_INCLUDED

#include <functional>
#include <ostream>

namespace cook {

enum class MessageType
{
    Success = 0x00,
    Warning = 0x01,
    Info = 0x02,
    Error = 0x04,
    InternalError = 0x008,
};

inline MessageType operator&(MessageType lhs, MessageType rhs)
{
    return static_cast<MessageType>(static_cast<int>(lhs) & static_cast<int>(rhs));
}
inline MessageType operator|(MessageType lhs, MessageType rhs)
{
    return static_cast<MessageType>(static_cast<int>(lhs) | static_cast<int>(rhs));
}
inline MessageType & operator|=(MessageType & lhs, MessageType rhs)
{
    lhs = lhs | rhs;
    return lhs;
}
inline MessageType & operator&=(MessageType & lhs, MessageType rhs)
{
    lhs = lhs & rhs;
    return lhs;
}
inline MessageType operator~(MessageType rhs)
{
    return static_cast<MessageType>(~static_cast<int>(rhs));
}



struct Message
{
    using Reporter = std::function<void (std::ostream &)>;

    explicit Message(MessageType message_type, Reporter report_functor = Reporter())
        : type(message_type),
          reporter(report_functor)
    {
    }

    MessageType type;
    Reporter reporter;
};

inline Message make_error_message(Message::Reporter reporter = Message::Reporter())             { return Message(MessageType::Error,            reporter); }
inline Message make_internal_error_message(Message::Reporter reporter = Message::Reporter())    { return Message(MessageType::InternalError,    reporter); }
inline Message make_warning_message(Message::Reporter reporter = Message::Reporter())           { return Message(MessageType::Warning,          reporter); }
inline Message make_info_message(Message::Reporter reporter = Message::Reporter())              { return Message(MessageType::Info,             reporter); }

}

#endif

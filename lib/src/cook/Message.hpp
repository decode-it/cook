#ifndef HEADER_cook_Message_hpp_ALREADY_INCLUDED
#define HEADER_cook_Message_hpp_ALREADY_INCLUDED

#include <functional>
#include <ostream>

namespace cook {

enum class MessageType
{
    Undefined = 0x00,
    Success = 0x01,
    Warning = 0x02,
    Error = 0x04
};

struct Message
{
    using Reporter = std::function<void (std::ostream &)>;

    Message()
        : type(MessageType::Undefined) {}

    explicit Message(MessageType type, Reporter reporter = Reporter())
        : type(type),
          reporter(reporter)
    {
    }

    MessageType type;
    Reporter reporter;
};

MessageType operator&(MessageType lhs, MessageType rhs)
{
    return static_cast<MessageType>(static_cast<int>(lhs) & static_cast<int>(rhs));
}
MessageType operator|(MessageType lhs, MessageType rhs)
{
    return static_cast<MessageType>(static_cast<int>(lhs) | static_cast<int>(rhs));
}
MessageType & operator|=(MessageType & lhs, MessageType rhs)
{
    lhs = lhs | rhs;
    return lhs;
}
MessageType & operator&=(MessageType & lhs, MessageType rhs)
{
    lhs = lhs & rhs;
    return lhs;
}
MessageType operator~(MessageType rhs)
{
    return static_cast<MessageType>(~static_cast<int>(rhs));
}

}

#endif

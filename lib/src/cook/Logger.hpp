#ifndef HEADER_cook_Logger_hpp_ALREADY_INCLUDED
#define HEADER_cook_Logger_hpp_ALREADY_INCLUDED

#include <functional>
#include <ostream>

namespace cook {

enum class MessageType
{
    Info,
    Warning,
    Error
};

inline std::ostream & operator<<(std::ostream & str, MessageType type)
{
    switch(type)
    {
#define L_CASE(NAME) case MessageType::NAME: return str << #NAME
    L_CASE(Info);
    L_CASE(Warning);
    L_CASE(Error);
#undef L_CASE
    default:
        return str << "unknown";
    }

}


class Logger
{
public:
    virtual ~Logger() {}

    using LogFunction = std::function<void (std::ostream &)>;

    virtual void log(MessageType type, const LogFunction & function) = 0;
};

}

#endif

#ifndef HEADER_cook_Logger_hpp_ALREADY_INCLUDED
#define HEADER_cook_Logger_hpp_ALREADY_INCLUDED

#include "cook/Message.hpp"
#include "cook/Result.hpp"
#include <functional>
#include <ostream>

namespace cook {

enum LogType
{
    Info,
    Warning,
    Error
};

inline std::ostream & operator<<(std::ostream &str, LogType type)
{
    switch(type)
    {
#define L_CASE(NAME) case LogType::NAME: return str << #NAME
    L_CASE(Info);
    L_CASE(Warning);
    L_CASE(Error);
#undef L_CASE
    default:
        return str << "unknown";
    }
}

#define LOG(TYPE, MSG) log(LogType::TYPE, [&](auto & os) {os << MSG; })

class Logger
{
public:
    virtual ~Logger() {}

    using LogFunction = std::function<void (std::ostream &)>;

    virtual void log(LogType type, const LogFunction & function) = 0;
    virtual void log(const Result & result) = 0;
};

}

#endif

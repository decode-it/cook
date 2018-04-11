#ifndef HEADER_cook_Logger_hpp_ALREADY_INCLUDED
#define HEADER_cook_Logger_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include <functional>
#include <ostream>

namespace cook {

class Logger
{
public:
    virtual ~Logger() {}

    virtual void log(const Result & result) const = 0;
    void log(Message::Type type, const std::string & msg)
    {
        log(Result() << Message(type, msg));
    }
    void log(Message::Type type, const std::string & msg, log::Ptr node)
    {
        log(Result() << Message(type, node, msg));
    }

};

}

#endif

#include "cook/Logger.hpp"
#include <iostream>

namespace cook {

namespace  {

std::ostream & operator<<(std::ostream & str, cook_NotificationType_t t)
{
    switch(t)
    {
#define L_CASE(NAME, STR) case cook_NotificationType_ ## NAME: str << STR; break
        L_CASE(Message, "Message");
        L_CASE(Info, "Info");
        L_CASE(Warning, "Warning");
        L_CASE(Error, "Error");
        L_CASE(Always, "Always");
#undef L_CASE

        default:
            str << "Unknown";
    }

    return str;
}

}

Logger::Logger()
    : cb_(&Logger::default_notification_callback),
      context_(nullptr)
{

}

Logger::Logger(cook_NotificationCallback_t cb, void * context)
    : cb_(cb),
      context_(context)
{

}

void Logger::set_callback(cook_NotificationCallback_t cb)
{
    cb_ = cb;
}

void Logger::default_notification_callback(void * context, cook_NotificationType_t type, const char * message)
{
    std::cout << type << ": " << message << std::endl;
}

}

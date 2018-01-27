#ifndef HEADER_cook_Logger_hpp_ALREADY_INCLUDED
#define HEADER_cook_Logger_hpp_ALREADY_INCLUDED

#include "cook/Notification.h"
#include <sstream>

namespace cook {

#define LOG(logger, TYPE, MSG) (logger).log(cook_NotificationType_ ## TYPE, [&](auto & os) { os << MSG; })

struct Logger
{
    Logger();
    explicit Logger(cook_NotificationCallback_t cb, void * context = nullptr);
    void set_callback(cook_NotificationCallback_t cb);
    static void default_notification_callback(void * context, cook_NotificationType_t type, const char * message);


    template <typename F>
    void log(cook_NotificationType_t type, F && fun)
    {
        std::ostringstream oss;
        fun(oss);
        cb_(context_, type, oss.str().c_str());
    }

    cook_NotificationCallback_t cb_;
    void * context_;
};

}

#endif

#ifndef HEADER_cook_view_Logger_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_Logger_hpp_ALREADY_INCLUDED

#include "cook/Notification.h"
#include <iostream>
#include <fstream>

namespace cook { namespace view { 

    class Logger
    {
    public:
        void set_verbose(unsigned int v)
        {
            verbose_ = v;
        }
        std::ostream &log(cook_NotificationType_t lt)
        {
            auto &os = std::cout;
            switch (lt)
            {
                case cook_NotificationType_Info:
                    if (verbose_ <= 1)
                        return dev_null_;
                    os << "Info: ";
                    break;

                case cook_NotificationType_Message:
                    if (verbose_ <= 0)
                        return dev_null_;
                    os << "Message: ";
                    break;

                case cook_NotificationType_Error: os << "Error: "; break;
                case cook_NotificationType_Warning: os << "Error: "; break;
                case cook_NotificationType_Fatal: os << "Error: "; break;
                case cook_NotificationType_Always: break;
            }
            return os;
        }
        std::ostream &log() {return log(cook_NotificationType_Message);}

    private:
        std::ofstream dev_null_;
        unsigned int verbose_ = 0;
    };

    // calback to forward cook notifications to the logger
    inline void log_callback(void * logger, cook_NotificationType_t type, const char * msg)
    {
        if (!logger)
            return;
        Logger & lg = *static_cast<Logger *>(logger);

        lg.log(type) << msg << std::endl;
    }

} } 

#endif

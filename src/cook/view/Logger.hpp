#ifndef HEADER_cook_view_Logger_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_Logger_hpp_ALREADY_INCLUDED

#include "cook/Types.hpp"
#include <iostream>
#include <fstream>

namespace cook { namespace view { 

    class Logger
    {
    public:
        std::ostream &log(LogType lt)
        {
            auto &os = std::cout;
            switch (lt)
            {
                case Info:
                    return dev_null_;
                    os << "Info: ";
                    break;
                case Message:
                    /* return dev_null_; */
                    break;
                case Error: os << "Error: "; break;
            }
            return os;
        }
        std::ostream &log() {return log(Message);}

    private:
        std::ofstream dev_null_;
    };

} } 

#endif

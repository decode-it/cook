#ifndef HEADER_cook_view_Logger_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_Logger_hpp_ALREADY_INCLUDED

#include "cook/Types.hpp"
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
        std::ostream &log(LogType lt)
        {
            auto &os = std::cout;
            switch (lt)
            {
                case Info:
                    if (verbose_ <= 1)
                        return dev_null_;
                    os << "Info: ";
                    break;
                case Message:
                    if (verbose_ <= 0)
                        return dev_null_;
                    os << "Message: ";
                    break;
                case Error: os << "Error: "; break;
            }
            return os;
        }
        std::ostream &log() {return log(Message);}

    private:
        std::ofstream dev_null_;
        unsigned int verbose_ = 0;
    };

} } 

#endif

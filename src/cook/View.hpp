#ifndef HEADER_cook_View_hpp_ALREADY_INCLUDED
#define HEADER_cook_View_hpp_ALREADY_INCLUDED

#include "cook/view/Logger.hpp"
#include "cook/presenter/Interface.hpp"
#include <iostream>

namespace cook { 

    class View
    {
    public:
        void inject(presenter::Interface *itf) { presenter_ = itf; }

        std::ostream &log(){return logger_.log();}
        std::ostream &log(LogType lt){return logger_.log(lt);}

        bool process_cli(int argc, const char **argv);

    private:
        presenter::Interface * presenter_;
        view::Logger logger_;
    };

} 

#endif

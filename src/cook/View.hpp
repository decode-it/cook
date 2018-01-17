#ifndef HEADER_cook_View_hpp_ALREADY_INCLUDED
#define HEADER_cook_View_hpp_ALREADY_INCLUDED

#include "cook/view/Logger.hpp"
#include <iostream>

namespace cook { 

struct Presenter;

    class View
    {
    public:
        void inject(Presenter *itf) { presenter_ = itf; }

        std::ostream &log(){return logger_.log();}
        std::ostream &log(LogType lt){return logger_.log(lt);}

        bool process_cli(int argc, const char **argv);

    private:
        Presenter * presenter_;
        view::Logger logger_;
    };

} 

#endif

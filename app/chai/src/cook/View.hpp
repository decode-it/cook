#ifndef HEADER_cook_View_hpp_ALREADY_INCLUDED
#define HEADER_cook_View_hpp_ALREADY_INCLUDED

#include "cook/view/Interface.hpp"
#include <iostream>

namespace cook { 

struct Presenter;

    class View : public view::Interface
    {
    public:
        void inject(Presenter *itf) { presenter_ = itf; }

        std::ostream &log() override            { return logger_.log(); }
        std::ostream &log(LogType lt) override  { return logger_.log(lt); }

        bool process_cli(int argc, const char **argv);

    private:
        Presenter * presenter_;
        view::Logger logger_;
    };

} 

#endif

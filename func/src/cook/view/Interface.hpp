#ifndef HEADER_cook_view_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_Interface_hpp_ALREADY_INCLUDED

#include "cook/view/Logger.hpp"
#include <iostream>

namespace cook {

struct Presenter;

namespace view {

    class Interface
    {
    public:
        virtual ~Interface() {}
        void inject(Presenter *itf) { presenter_ = itf; }

        virtual std::ostream & log() = 0;
        virtual std::ostream & log(LogType lt) = 0;

    private:
        Presenter * presenter_;
    };

} }

#endif

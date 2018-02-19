#ifndef HEADER_cook_App_hpp_ALREADY_INCLUDED
#define HEADER_cook_App_hpp_ALREADY_INCLUDED

#include "cook/app/Options.hpp"

namespace cook {

class App
{
public:
    bool initialize(const app::Options & options);
    bool process();

private:
    app::Options options_;
};

}

#endif

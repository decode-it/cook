#ifndef HEADER_cook_main_hpp_ALREADY_INCLUDED
#define HEADER_cook_main_hpp_ALREADY_INCLUDED

#include "cook/Codes.hpp"
#include "cook/recipe/Loader.hpp"
#include "gubg/mss.hpp"

namespace cook { 
    ReturnCode main(int argc, const char **argv)
    {
        MSS_BEGIN(ReturnCode);

        recipe::Loader loader;
        MSS(loader.load("recipes.chai"));

        MSS_END();
    }
} 

#endif

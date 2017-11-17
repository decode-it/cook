#ifndef HEADER_cook_main_hpp_ALREADY_INCLUDED
#define HEADER_cook_main_hpp_ALREADY_INCLUDED

#include "cook/Model.hpp"
#include "cook/View.hpp"
#include "cook/Presenter.hpp"
#include "cook/Codes.hpp"
#include "cook/view/Options.hpp"
#include "gubg/mss.hpp"
#include "gubg/file/System.hpp"
#include "gubg/std/filesystem.hpp"
#include <fstream>
#include <set>
#include <cstdlib>

namespace cook { 
   
    ReturnCode main(int argc, const char **argv)
    {
        MSS_BEGIN(ReturnCode);

        Model model;
        View view;
        Presenter presenter(model, view);

        MSS(view.process_cli(argc, argv));
        
        MSS_END();
    }
} 

#endif

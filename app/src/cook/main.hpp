#ifndef HEADER_cook_main_hpp_ALREADY_INCLUDED
#define HEADER_cook_main_hpp_ALREADY_INCLUDED

#include "cook/API.h"
#include "cook/Model.hpp"
#include "cook/View.hpp"
#include "cook/Codes.hpp"
#include "cook/view/Options.hpp"
#include "gubg/mss.hpp"
#include "gubg/file/System.hpp"
#include "gubg/std/filesystem.hpp"
#include <fstream>
#include <set>
#include <cstdlib>

namespace cook { 

cook_Handle_t create_api(View & view)
{
    std::string cur_path = std::filesystem::current_path().string();

    cook_API_Parameters_t params;
    params.callback = &view::log_callback;
    params.callback_context = static_cast<void*>(&view.logger_);
    params.project_directory = cur_path.c_str();

    return cook_API_create(&params);
}

ReturnCode main(int argc, const char **argv)
{
    MSS_BEGIN(ReturnCode);
    View view;

    // initialize the api
    cook_Handle_t api = create_api(view);
    MSS(!!api, std::cout << "Unable to create the api" << std::endl);
    MSS(view.process_cli(api, argc, argv));

    cook_API_destroy(api);

    MSS_END();
}
} 

#endif

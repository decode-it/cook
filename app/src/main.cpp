#include "cook/App.hpp"
#include "gubg/mss.hpp"
#include <iostream>

using namespace cook;

enum ReturnCode
{
    OK = 0,
    Error = 1
};

int main(int argc, const char ** argv)
{
    MSS_BEGIN(ReturnCode);

    // first parse the options
    app::Options options;
    {
        MSS(options.parse(argc, argv), std::cerr << options.help_message << std::endl);

        if (options.print_help)
        {
            std::cout << options.help_message << std::endl;
            MSS_RETURN_OK();
        }
    }

    App app;
    MSS(app.initialize(options), std::cerr << "Error initializing application" << std::endl);
    MSS(app.process());

    std::cout << std::endl;

    MSS_END();
}

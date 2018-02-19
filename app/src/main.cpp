#include "cook/App.hpp"
#include "gubg/mss.hpp"
#include <iostream>

using namespace cook;

enum ReturnCode
{
    OK = 0,
    Error = 1
};

int main(int argc, char ** argv)
{
    MSS_BEGIN(ReturnCode);

    app::Options options;

    MSS(options.parse(argc, argv), std::cerr << "Unable to parse the arguments" << std::endl);

    App app;
    MSS(app.initialize(options), std::cerr << "Error initializing application" << std::endl);
    MSS(app.process());


    MSS_END();
}

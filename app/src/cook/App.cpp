#include "cook/App.hpp"
#include "cook/chai/Kitchen.hpp"
#include "gubg/mss.hpp"

namespace cook {

bool App::initialize(const app::Options & options)
{
    MSS_BEGIN(bool);
    MSS(options.valid());

    options_ = options;
    MSS_END();
}

bool App::process()
{
    MSS_BEGIN(bool);

    // process all files
    cook::chai::Kitchen kitchen;

    MSS(kitchen.add_variables(options_.variables));
    MSS(kitchen.load(options_.recipe_files));

    // resolve the root book
    MSS_END();
}

}

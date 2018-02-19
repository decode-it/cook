#include "cook/app/Options.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace app {

bool Options::parse(int argc, char ** argv)
{
    MSS_BEGIN(bool);
    MSS(!parsed_);

    if (recipe_files.empty())
        recipe_files.push_back("recipes.chai");

    parsed_ = true;
    MSS_END();
}

bool Options::valid() const
{
    return parsed_;
}

} }

#include "cook/Kitchen.hpp"
#include "gubg/mss.hpp"

namespace cook {

bool Kitchen::register_toolchain(ToolChainPtr toolchain)
{
    MSS_BEGIN(bool);

    MSS(!!toolchain);
    const std::string & name = toolchain->name();
    MSS(!name.empty());
    MSS(toolchains_.find(name) == toolchains_.end());

    toolchains_.emplace(name, toolchain);

    MSS_END();
}

bool Kitchen::resolve_dependencies()
{
    MSS_BEGIN(bool);




    MSS_END();
}

}


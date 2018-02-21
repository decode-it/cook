#include "cook/Kitchen.hpp"
#include "gubg/mss.hpp"
#include <cassert>

namespace cook {

Kitchen::Kitchen()
{
}

bool Kitchen::initialize()
{
    MSS_BEGIN(bool);
    default_context_.environment = create_environment();
    MSS(!!context().environment);

    MSS_END();

}

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

model::Context & Kitchen::context()
{
    return default_context_;
}

const model::Context & Kitchen::context() const
{
    return default_context_;
}

bool Kitchen::add_variables(const std::list<Variable> & variables)
{
    MSS_BEGIN(bool);

    MSS(!!default_context_.environment);
    MSS(default_context_.environment->set_variables(variables));

    MSS_END();
}

bool Kitchen::resolve_dependencies()
{
    MSS_BEGIN(bool);




    MSS_END();
}

}


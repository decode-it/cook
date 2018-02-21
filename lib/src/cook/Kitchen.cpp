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
    default_context_.environment = create_environment( std::shared_ptr<model::Environment>() );
    MSS(!!default_context().environment);

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

model::Context & Kitchen::default_context()
{
    return default_context_;
}

const model::Context & Kitchen::default_context() const
{
    return default_context_;
}

bool Kitchen::has_context(const std::string & name) const
{
    return registered_contexts_.find(name) != registered_contexts_.end();
}

model::Context & Kitchen::context(const std::string & name)
{
    return registered_contexts_[name];
}

const model::Context & Kitchen::context(const std::string & name) const
{
    auto it = registered_contexts_.find(name);
    assert(it != registered_contexts_.end());

    return it->second;
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


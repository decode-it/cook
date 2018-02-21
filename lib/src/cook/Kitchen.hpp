#ifndef HEADER_cook_Kitchen_hpp_ALREADY_INCLUDED
#define HEADER_cook_Kitchen_hpp_ALREADY_INCLUDED

#include "cook/Logger.hpp"
#include "cook/model/Book.hpp"
#include "cook/toolchain/Interface.hpp"
#include "cook/model/Context.hpp"

namespace cook {

class Kitchen
{
public:
    using ToolChainPtr = std::shared_ptr<toolchain::Interface>;
    using Variable = std::pair<std::string, std::string>;

    Kitchen();
    virtual ~Kitchen() {}

    bool initialize();


    bool register_toolchain(ToolChainPtr toolchain);

    model::Context & context();
    const model::Context & context() const;

    model::Book & root_book()               { return context().root; }
    const model::Book & root_book() const   { return context().root; }

    bool add_variables(const std::list<Variable> & variables);
    virtual Logger & logger() = 0;

    bool resolve_dependencies();

private:
    virtual std::shared_ptr<model::Environment> create_environment() const = 0;

    model::Context default_context_;
    std::map<std::string, ToolChainPtr> toolchains_;
};

}

#endif

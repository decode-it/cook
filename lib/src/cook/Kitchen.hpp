#ifndef HEADER_cook_Kitchen_hpp_ALREADY_INCLUDED
#define HEADER_cook_Kitchen_hpp_ALREADY_INCLUDED

#include "cook/Logger.hpp"
#include "cook/model/Book.hpp"
#include "cook/toolchain/Interface.hpp"

namespace cook {

class Kitchen
{
public:
    using ToolChainPtr = std::shared_ptr<toolchain::Interface>;
    using Variable = std::pair<std::string, std::string>;

    virtual ~Kitchen() {}

    bool register_toolchain(ToolChainPtr toolchain);
    model::Book & root_book()               { return root_; }
    const model::Book & root_book() const   { return root_; }

    virtual bool add_variables(const std::list<Variable> & variables) = 0;
    virtual Logger & logger() = 0;

    bool resolve_dependencies();

private:
    std::map<std::string, ToolChainPtr> toolchains_;
    model::Book root_;
};

}

#endif

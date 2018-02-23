#ifndef HEADER_cook_Kitchen_hpp_ALREADY_INCLUDED
#define HEADER_cook_Kitchen_hpp_ALREADY_INCLUDED

#include "cook/Logger.hpp"
#include "cook/model/Book.hpp"
#include "cook/toolchain/Interface.hpp"
#include "cook/model/Context.hpp"

namespace cook {

class Kitchen
{
    struct Context : public model::Context
    {
        Context() { root = &actual_root; }
        model::Book actual_root;
    };

public:
    using ToolChainPtr = std::shared_ptr<toolchain::Interface>;
    using Variable = std::pair<std::string, std::string>;

    Kitchen();
    virtual ~Kitchen() {}

    bool initialize();

    virtual Logger & logger() = 0;

    // getters and setter
    model::Context & context();
    const model::Context & context() const;
    model::Book * root_book() const;


    Result register_toolchain(ToolChainPtr toolchain);
    Result register_variable(const std::string & name, const std::string & value);


    Result find_recipe(model::Recipe *&recipe, const std::string & name) const;

    Result resolve_dependencies();

private:
    virtual std::shared_ptr<model::Environment> create_environment() const = 0;
    std::map<std::string, ToolChainPtr> toolchains_;

    Context context_;
};

}

#endif

#ifndef HEADER_cook_toolchain_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_toolchain_Interface_hpp_ALREADY_INCLUDED

#include "cook/chef/Interface.hpp"
#include <memory>

namespace cook { namespace toolchain {


using ChefPtr = std::shared_ptr<chef::Interface>;

struct Step
{
    std::string name;
    std::string description;
    std::vector<ChefPtr> chefs;
};

class Interface
{
public:
    virtual std::string name() const = 0;

    virtual Result generate_processors(model::Recipe * recipe, std::list<Step> & steps) = 0;
};

} }

#endif

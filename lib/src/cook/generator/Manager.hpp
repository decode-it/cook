#ifndef HEADER_cook_generator_Manager_hpp_ALREADY_INCLUDED
#define HEADER_cook_generator_Manager_hpp_ALREADY_INCLUDED

#include "cook/generator/Interface.hpp"
#include "cook/api/GenerationType.h"
#include <map>
#include <memory>

namespace cook { namespace generator {

class Manager
{
public:
    using FactoryPtr = std::shared_ptr<Factory>;

    Manager();

    bool has_generator(cook_api_GenerationType_t key) const;
    FactoryPtr find_generator(cook_api_GenerationType_t key) const;

private:
    std::map<cook_api_GenerationType_t, FactoryPtr> generators_;
};


} }

#endif

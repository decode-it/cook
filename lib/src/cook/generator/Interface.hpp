#ifndef HEADER_cook_generator_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_generator_Interface_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include <memory>

namespace cook {

class Context;

}

namespace cook { namespace generator {

class Interface
{
public:
    virtual std::string name() const = 0;

    virtual Result set_option(const std::string & option) = 0;

    virtual bool can_process(const Context & context) const = 0;
    virtual Result process(const Context & context) = 0;
};

} }

#endif

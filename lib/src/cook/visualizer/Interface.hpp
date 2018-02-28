#ifndef HEADER_cook_visualizer_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_visualizer_Interface_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include <memory>

namespace cook {

class Context;

namespace model {

class Menu;

} }

namespace cook { namespace visualizer {

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

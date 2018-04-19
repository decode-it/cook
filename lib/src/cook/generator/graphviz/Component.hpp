#ifndef HEADER_cook_generator_graphviz_Component_hpp_ALREADY_INCLUDED
#define HEADER_cook_generator_graphviz_Component_hpp_ALREADY_INCLUDED

#include "cook/generator/Interface.hpp"

namespace cook { namespace generator { namespace graphviz {

class Component : public Interface
{
public:
    std::string name() const override;

    Result set_option(const std::string & option) override;

    bool can_process(const Context & context) const override;
    Result process(const Context & context) override;

    std::string default_filename() const override;

};

} } }

#endif

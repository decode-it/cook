#ifndef HEADER_cook_visualizer_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_visualizer_Interface_hpp_ALREADY_INCLUDED

#include "cook/model/Menu.hpp"
#include "cook/model/Environment.hpp"
#include "cook/Result.hpp"
#include <memory>

namespace cook { namespace visualizer {

class Interface
{
public:
    virtual std::string name() const = 0;

    virtual Result set_option(const std::string & option) = 0;

    virtual bool can_process(const model::DependencyGraph & graph) const = 0;
    virtual Result process(const model::DependencyGraph & graph, const model::Environment & environment) = 0;
};

} }

#endif

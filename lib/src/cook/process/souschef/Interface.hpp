#ifndef HEADER_cook_process_chef_Assistant_hpp_ALREADY_DEFINED
#define HEADER_cook_process_chef_Assistant_hpp_ALREADY_DEFINED

#include "cook/process/souschef/Context.hpp"

namespace cook { namespace process { namespace souschef {

class Interface
{
public:
    virtual ~Interface() {}

    virtual Result process(const Context & context, model::Snapshot & snapshot) const = 0;
    virtual std::string description() const  = 0;
};



} } }

#endif

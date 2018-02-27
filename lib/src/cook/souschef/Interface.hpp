#ifndef HEADER_cook_chef_Assistant_hpp_ALREADY_DEFINED
#define HEADER_cook_chef_Assistant_hpp_ALREADY_DEFINED

#include "cook/model/Recipe.hpp"
#include "cook/model/Environment.hpp"
#include <vector>
#include <list>

namespace cook { namespace souschef {

struct Context
{
    model::Recipe * recipe;
    std::vector<model::Recipe *> dependencies;
    std::list<model::Recipe *> topological_order;

    model::Environment * environment;
};

class Interface
{
public:
    virtual ~Interface() {}

    virtual Result process(const Context & context, model::Snapshot & snapshot) const = 0;
    virtual std::string description() const  = 0;
};



} }

#endif

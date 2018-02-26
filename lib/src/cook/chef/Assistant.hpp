#ifndef HEADER_cook_chef_Assistant_hpp_ALREADY_DEFINED
#define HEADER_cook_chef_Assistant_hpp_ALREADY_DEFINED

#include "cook/model/Recipe.hpp"
#include "cook/model/Environment.hpp"
#include <vector>
#include <list>

namespace cook { namespace chef {

struct Context
{
    model::Recipe * recipe;
    std::vector<model::Recipe *> dependencies;
    std::list<model::Recipe *> topological_order;

    model::Environment * environment;
};

class Assistant
{
public:
    virtual ~Assistant() {}

    virtual Result process(const Context & context, model::Snapshot & snapshot) const = 0;
    virtual std::string description() const  = 0;
};

using AssistantPtr = std::shared_ptr<Assistant>;

} }

#endif

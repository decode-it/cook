#ifndef HEADER_cook_model_DependencyGraph_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_DependencyGraph_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"
#include "gubg/network/DAG.hpp"
#include <map>

namespace cook { namespace model {

class DependencyGraph
{
public:
    using ResolvedDependencies = std::map<Recipe::Dependency, Recipe *>;

    explicit DependencyGraph(Recipe * root);
    bool resolve();

    std::vector<Recipe *> topological_order();

private:
    bool resolve_dependency_(Recipe *& dependency, const Recipe * recipe, const Recipe::Dependency & uri);

    gubg::network::DAG<Recipe> network_;
    Recipe * root_vertex_;
};

} }



#endif

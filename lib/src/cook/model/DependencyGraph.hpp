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
    bool resolve(bool &success);

    std::vector<Recipe *> topological_order();

private:
    Recipe * resolve_relative_dependency_(Book * book, const Uri & uri);
    Recipe * resolve_absolute_dependency_(Book * book, const Uri & uri);

    gubg::network::DAG<Recipe> network_;
    Recipe * root_vertex_;
};

} }



#endif

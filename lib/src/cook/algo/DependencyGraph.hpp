#ifndef HEADER_cook_algo_DependencyGraph_hpp_ALREADY_INCLUDED
#define HEADER_cook_algo_DependencyGraph_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace algo {

class DependencyGraph
{
    enum ConstructionResult
    {
        OK                      = 0b000,
        UnresolvedDependencies  = 0b001,
        CyclicDependencies      = 0b010,
        InternalError           = 0b111
    };

public:
    void clear();

    template <typename It> Result construct(gubg::Range<It> root_recipes)
    {
        MSS_BEGIN(Result);
        clear();
        root_recipes_.assign(RANGE(root_recipes));

        MSS(construct_());
        MSS_END();
    }

    bool all_dependencies_resolved() const;
    bool is_acyclic() const;

    static bool topological_suborder(model::Recipe * new_root, const std::list<model::Recipe*> & topological_order, std::list<model::Recipe *> & new_order);

    bool topological_order(model::Recipe * root, std::list<model::Recipe *> & order) const;
    const std::list<model::Recipe *> &topological_order() const;

private:
    Result construct_();
    Result resolve_recursive_dependencies_(bool & all_resolved);
    Result sort_topologically_(bool & acyclic);

    std::list<model::Recipe *> root_recipes_;
    std::list<model::Recipe *> topological_order_;
    ConstructionResult result_;
};


} }

#endif

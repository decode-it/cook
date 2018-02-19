#ifndef HEADER_cook_algo_DependencyResolver_hpp_ALREADY_INCLUDED
#define HEADER_cook_algo_DependencyResolver_hpp_ALREADY_INCLUDED

#include "cook/model/Book.hpp"
#include <vector>
#include <map>

namespace cook { namespace algo {

enum class DependencyResolveResult
{
    Success,
    Unresolved_Dependencies,
    Cyclic_Dependencies,
    Internal_Error
};

struct DependencyResolver
{
    using result_type = DependencyResolveResult;

    DependencyResolver(model::Book * root) : root_(root) {}

    DependencyResolveResult resolve(const std::list<model::Recipe *> & recipe_roots);

    const std::vector<model::Recipe *> & topological_order() const;
    std::vector<model::Recipe *> topological_suborder(model::Recipe * subroot) const;

private:
    using CountMap = std::map<model::Recipe *, unsigned int>;

    result_type fill_count_map_(CountMap & map, const std::list<model::Recipe *> &recipe_roots);
    result_type topological_sort_(CountMap & map);

    model::Book * root_;
    std::vector<model::Recipe*> order_;

};

} }

#endif

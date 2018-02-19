#ifndef HEADER_cook_algo_DependencyResolver_hpp_ALREADY_INCLUDED
#define HEADER_cook_algo_DependencyResolver_hpp_ALREADY_INCLUDED

#include "cook/model/Book.hpp"
#include <vector>
#include <map>

namespace cook { namespace algo {

struct DependencyResolver
{
    enum result_type
    {
        success,
        unresolved_dependencies,
        cyclic_dependencies,
        internal_error
    };

    DependencyResolver(model::Book * root) : root_(root) {}

    result_type resolve(const std::list<model::Recipe *> & recipe_roots);

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

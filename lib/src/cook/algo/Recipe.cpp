#include "cook/algo/Recipe.hpp"
#include "cook/algo/Visit.hpp"
#include <unordered_map>
#include <stack>

namespace cook { namespace algo {

namespace {

using DegreeMap = std::unordered_map<model::Recipe *, unsigned int>;

template <typename It>
Result fill_in_degree_map_(const gubg::Range<It> & root_range, DegreeMap & degree_map)
{
    MSS_BEGIN(Result);

    MSS(degree_map.empty());

    auto init = [&](auto & stack)
    {
        for(model::Recipe * recipe: root_range)
        {
            stack.push(recipe);
            degree_map[recipe] = 0;
        }
    };

    Result rc;

    auto process = [&](model::Recipe * cur, auto & todo)
    {
        MSS_BEGIN(Result);
        MSS(!!cur);

        for(const auto & p : cur->dependency_pairs())
        {
            model::Recipe * dep = p.second;

            if (dep == nullptr)
                rc << MESSAGE(Error, "Recipe " << cur->uri() << " has a unresolved dependency on " << p.first);
            else
            {

                ++degree_map[dep];
                todo.push(dep);
            }
        }

        MSS_END();
    };

    MSS(visit(init, process));
    MSS(rc);

    MSS_END();
}

template <typename It>
Result order_topologically_(DegreeMap & map, It out)
{
    MSS_BEGIN(Result);

    auto initialization = [&](auto & stack)
    {
        for (const auto & p : map)
            if (p.second == 0)
                stack.push(p.first);
    };

    auto process = [&](model::Recipe * cur, auto & stack)
    {
        MSS_BEGIN(Result);

        MSS(map[cur] == 0);
        *out++ = cur;

        for(model::Recipe * dep : cur->dependencies())
        {
            MSS(!!dep);
            auto & in_count = map[dep];
            MSS(in_count > 0);

            if (--in_count == 0)
                stack.push(dep);
        }

        MSS_END();
    };

    MSS(visit(initialization, process));
    MSG_MSS(std::all_of(RANGE(map), [](const auto & p) { return p.second == 0; }), Error, "Cyclic dependency detected");

    MSS_END();
}

template <typename It>
Result topological_order_(const gubg::Range<It> & root_range, std::list<model::Recipe*> & top_order)
{
    MSS_BEGIN(Result);

    DegreeMap in_degree_map;

    MSS(fill_in_degree_map_(root_range, in_degree_map));
    MSS(order_topologically_(in_degree_map, std::back_inserter(top_order)));

    MSS_END();
}

}

Result topological_order(const std::list<model::Recipe *> & roots, std::list<model::Recipe*> & top_order)
{
    return topological_order_(gubg::make_range(roots), top_order);
}
Result topological_order(model::Recipe * root, std::list<model::Recipe*> & top_order)
{
    return topological_order_(gubg::make_range(&root, (&root) + 1), top_order);
}


} }


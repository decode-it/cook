#ifndef HEADER_cook_algo_DependencyGraph_hpp_ALREADY_INCLUDED
#define HEADER_cook_algo_DependencyGraph_hpp_ALREADY_INCLUDED

#include "cook/algo/Visit.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/Result.hpp"
#include "boost/graph/adjacency_list.hpp"

namespace cook { namespace algo {

template <typename It, typename Graph, typename RecipeVertexMap>
Result make_DependencyGraph(const gubg::Range<It> & roots, Graph & g, RecipeVertexMap & translation_map)
{
    MSS_BEGIN(Result);

    MSS(boost::num_vertices(g) == 0);
    MSS(translation_map.empty());

    auto initialization_function = [&](auto & stack)
    {
        for(model::Recipe * root : roots)
            if(translation_map.find(root) == translation_map.end())
            {
                stack.push(root);
                translation_map.emplace(root, boost::add_vertex(root, g));
            }
    };

    Result rc;
    auto process_function = [&](model::Recipe * recipe, auto & stack)
    {
        MSS_BEGIN(Result);

        MSS(!!recipe);
        typename boost::graph_traits<Graph>::vertex_descriptor vd;
        {
            auto it = translation_map.find(recipe);
            MSS(it != translation_map.end());
            vd = it->second;
        }

        for (const auto & uri_dep_pair : recipe->dependency_pairs())
        {
            model::Recipe * dep = uri_dep_pair.second.recipe;

            if (dep == nullptr)
                rc << MESSAGE(Error, "Recipe " << recipe->uri() << " has a unresolved dependency on " << uri_dep_pair.first);
            else
            {
                // insert  a dummy
                auto it_bool_pair = translation_map.emplace(dep, boost::graph_traits<Graph>::null_vertex());

                // it is a new entry
                if (it_bool_pair.second)
                    it_bool_pair.first->second = boost::add_vertex(dep, g);

                boost::add_edge(vd, it_bool_pair.first->second, g);

                stack.push(dep);
            }
        }

        MSS_END();
    };

    MSS(visit(initialization_function, process_function));
    MSS(rc);

    MSS_END();
}

template <typename It, typename Graph>
Result make_DependencyGraph(const gubg::Range<It> & roots, Graph & g)
{
    MSS_BEGIN(Result);

    std::unordered_map<model::Recipe *, typename boost::graph_traits<Graph>::vertex_descriptor> translation_map;
    MSS(make_DependencyGraph(roots, g, translation_map));
    MSS_END();
}

} }

#endif

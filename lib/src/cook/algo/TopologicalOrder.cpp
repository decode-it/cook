#include "cook/algo/TopologicalOrder.hpp"
#include "cook/algo/DependencyGraph.hpp"
#include "boost/graph/topological_sort.hpp"
#include <unordered_map>
#include <stack>

namespace cook { namespace algo {

namespace {

using G = boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS, model::Recipe *>;
using Vertex = boost::graph_traits<G>::vertex_descriptor;

template <typename It>
Result topological_order_(const gubg::Range<It> & root_range, std::list<model::Recipe*> & top_order)
{
    MSS_BEGIN(Result);

    G g;
    std::unordered_map<model::Recipe *, boost::graph_traits<G>::vertex_descriptor> map;
    MSS(make_DependencyGraph(root_range, g, map));
    MSS(make_TopologicalOrder(g, std::back_inserter(top_order)));

    MSS_END();
}

}

Result make_TopologicalOrder(const std::list<model::Recipe *> & roots, std::list<model::Recipe*> & top_order)
{
    return topological_order_(gubg::make_range(roots), top_order);
}
Result make_TopologicalOrder(model::Recipe * root, std::list<model::Recipe*> & top_order)
{
    return topological_order_(gubg::make_range(&root, (&root) + 1), top_order);
}


} }


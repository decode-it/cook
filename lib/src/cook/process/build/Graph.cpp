#include "cook/process/build/Graph.hpp"
#include "boost/graph/topological_sort.hpp"

namespace cook { namespace process { namespace build {

Graph::vertex_descriptor Graph::goc_vertex(const FileLabel & path)
{
    auto it = file_map_.find(path);
    if (it != file_map_.end())
        return it->second;

    auto v = boost::add_vertex(Label(path), g_);
    file_map_.insert(std::make_pair(path, v));

    return v;
}

Graph::vertex_descriptor Graph::add_vertex(CommandLabel ptr)
{
    boost::add_vertex(Label(ptr), g_);
}

Result Graph::add_edge(vertex_descriptor src, vertex_descriptor tgt)
{
    MSS_BEGIN(Result);

    const Label & src_label = g_[src];
    const Label & tgt_label = g_[tgt];

    MSS(src_label.index() !=  tgt_label.index());

    if (std::holds_alternative<FileLabel>(tgt_label))
    {
        auto p = boost::in_edges(tgt, g_);
        MSG_MSS(p.first == p.second, InternalError, "file " << std::get<FileLabel>(tgt_label) << " is already generated by another command");
    }

    MSS(boost::add_edge(src, tgt, g_).second);

    MSS_END();

}

const Graph::Label & Graph::operator[](vertex_descriptor vd) const
{
    return g_[vd];
}

Result Graph::topological_commands(std::vector<vertex_descriptor> & commands) const
{
    MSS_BEGIN(Result);
    std::vector<vertex_descriptor> top_order(boost::num_vertices(g_));

    try
    {
        boost::topological_sort(g_, top_order.begin());
    }
    catch(boost::not_a_dag)
    {
        MSG_MSS(false, InternalError, "The execution graph is not acyclic");
    }

    commands.clear();
    for(vertex_descriptor vd : top_order)
    {
        const Label & l = g_[vd];
        auto visitor = [&](auto && arg)
        {
            using T = decltype(arg);
            if (std::is_same<T, CommandLabel>::value)
                commands.push_back(vd);
        };
        std::visit(visitor, l);
    }

    MSS_END();
}

} } }


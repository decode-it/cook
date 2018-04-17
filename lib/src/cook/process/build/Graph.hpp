#ifndef HEADER_cook_process_build_Graph_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_build_Graph_hpp_ALREADY_INCLUDED

#include "cook/process/command/Interface.hpp"
#include "cook/model/Uri.hpp"
#include "boost/graph/adjacency_list.hpp"
#include "boost/variant.hpp"
#include <memory>
#include <vector>
#include <list>

namespace cook { namespace process { namespace build {

namespace config {

struct Graph
{
    using FileLabel = std::filesystem::path;
    using CommandLabel = command::Ptr;

    using Label = boost::variant<FileLabel, CommandLabel>;

    enum EdgeType
    {
        Explicit = 0x01,
        Implicit = 0x02,
    };
    using graph_type = boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS, Label, EdgeType>;
    using vertex_descriptor = boost::graph_traits<graph_type>::vertex_descriptor;
    using adjacent_descriptor = boost::graph_traits<graph_type>::vertex_descriptor;

    using OrderedVertices = std::vector<vertex_descriptor>;
};

}

struct Graph : public config::Graph
{
    Graph() = default;

    size_t num_vertices() const {return boost::num_vertices(g_);}

    vertex_descriptor goc_vertex(const FileLabel & path);
    vertex_descriptor add_vertex(CommandLabel ptr);
    Result add_edge(vertex_descriptor consumer, vertex_descriptor producer, EdgeType type = Explicit);

    Result topological_commands(std::vector<vertex_descriptor> & commands) const;

    const Label & operator[](vertex_descriptor vd) const;

    template <typename It>
    void input(It it, vertex_descriptor command, EdgeType required = EdgeType()) const
    {
        for(auto e : gubg::make_range(boost::out_edges(command, g_)))
            if ((g_[e] & required) == required)
                *it++ = boost::target(e, g_);
    }

    template <typename It>
    void output(It it, vertex_descriptor command, EdgeType required = EdgeType()) const
    {
        for(auto e : gubg::make_range(boost::in_edges(command, g_)))
            if ((g_[e] & required) == required)
                *it++ = boost::source(e, g_);
    }

private:
    Graph(const Graph &) = delete;
    Graph & operator=(const Graph &) = delete;
    Graph(Graph &&) = delete;
    Graph & operator=(Graph &&) = delete;

    std::unordered_map<FileLabel, vertex_descriptor> file_map_;
    graph_type g_;
};

using GraphPtr = std::shared_ptr<Graph>;

} } }

#endif

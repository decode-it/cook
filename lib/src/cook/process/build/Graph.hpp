#ifndef HEADER_cook_process_build_Graph_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_build_Graph_hpp_ALREADY_INCLUDED

#include "cook/process/command/Interface.hpp"
#include "cook/model/Uri.hpp"
#include "gubg/graph/AdjacencyList.hpp"
#include <variant>
#include <memory>
#include <vector>
#include <list>

namespace cook { namespace process { namespace build {

namespace config {

struct Graph
{
    using FileLabel = std::filesystem::path;
    using CommandLabel = command::Ptr;

    using Label = std::variant<FileLabel, CommandLabel>;

    enum EdgeType
    {
        Explicit = 0x01,
        Implicit = 0x02,
    };
    using graph_type = gubg::graph::AdjacencyList<gubg::graph::use_vector, gubg::graph::use_list, gubg::graph::use_list, Label, EdgeType, gubg::graph::bidirectional>;
    using vertex_descriptor = gubg::graph::Traits<graph_type>::vertex_descriptor;
    using adjacent_descriptor = gubg::graph::Traits<graph_type>::vertex_descriptor;

    using OrderedVertices = std::vector<vertex_descriptor>;
};

}

struct Graph : public config::Graph
{
    Graph() = default;

    size_t num_vertices() const {return gubg::graph::num_vertices(g_);}

    vertex_descriptor goc_vertex(const FileLabel & path);
    vertex_descriptor add_vertex(CommandLabel ptr);
    Result add_edge(vertex_descriptor consumer, vertex_descriptor producer, EdgeType type = Explicit);

    Result topological_commands(std::vector<vertex_descriptor> & commands) const;

    const Label & operator[](vertex_descriptor vd) const;

    template <typename Functor>
    void input(Functor && functor, vertex_descriptor command, EdgeType required = EdgeType()) const
    {
        for(auto e : gubg::graph::out_edges(command, g_))
            if ((gubg::graph::edge_label(e, g_) & required) == required)
                functor(gubg::graph::target(e, g_));
    }

    template <typename Functor>
    void output(Functor && functor, vertex_descriptor command, EdgeType required = EdgeType()) const
    {
        for(auto e : gubg::graph::in_edges(command, g_))
            if ((gubg::graph::edge_label(e, g_) & required) == required)
                functor(gubg::graph::source(e, g_));
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

#ifndef HEADER_cook_process_DependencyDAG_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_DependencyDAG_hpp_ALREADY_INCLUDED

#include "cook/process/Command.hpp"
#include "cook/model/Uri.hpp"
#include "boost/graph/adjacency_list.hpp"
#include <variant>

namespace cook { namespace process {

struct DependencyDAG
{
    using FileLabel = std::filesystem::path;
    using CommandLabel = CommandPtr;

    using Label = std::variant<FileLabel, CommandLabel>;
    using graph_type = boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS, Label>;
    using vertex_descriptor = boost::graph_traits<graph_type>::vertex_descriptor;
    using adjcent_descriptor = boost::graph_traits<graph_type>::vertex_descriptor;

    DependencyDAG() = default;

    vertex_descriptor goc_vertex(const FileLabel & path);
    vertex_descriptor add_vertex(CommandLabel ptr);
    Result add_edge(vertex_descriptor src, vertex_descriptor tgt);

    Result topological_commands(std::vector<vertex_descriptor> & commands) const;

    const Label & operator[](vertex_descriptor vd) const;

private:
    DependencyDAG(const DependencyDAG &) = delete;
    DependencyDAG & operator=(const DependencyDAG &) = delete;
    DependencyDAG(DependencyDAG &&) = delete;
    DependencyDAG & operator=(DependencyDAG &&) = delete;

    std::unordered_map<FileLabel, vertex_descriptor> file_map_;
    graph_type g_;
};

} }

#endif
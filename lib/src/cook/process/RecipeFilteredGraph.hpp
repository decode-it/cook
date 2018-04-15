#ifndef HEADER_cook_process_RecipeFilteredGraph_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_RecipeFilteredGraph_hpp_ALREADY_INCLUDED

#include "cook/process/build/Graph.hpp"

namespace cook { namespace process {

class RecipeFilteredGraph : public build::config::Graph
{
    using CommandVertices = std::unordered_set<vertex_descriptor>;
    using CommandVertexIterator = CommandVertices::const_iterator;
public:
    explicit RecipeFilteredGraph(build::GraphPtr ptr);

    size_t num_vertices() const
    {
        if (!ptr_)
            return 0;
        return ptr_->num_vertices();
    }

    vertex_descriptor goc_vertex(const FileLabel & path);
    vertex_descriptor add_vertex(CommandLabel ptr);
    Result add_edge(vertex_descriptor consumer, vertex_descriptor producer);

    Result topological_commands(OrderedVertices & commands) const;

    const Label & operator[](vertex_descriptor vd) const;

    gubg::Range<CommandVertexIterator> command_vertices() const;

    template <typename InIt, typename OutIt>
    void input_output(InIt in, OutIt out, vertex_descriptor command) const
    {
        if (command_vertices_.find(command) == command_vertices_.end())
            return;

        ptr_->input_output(in, out, command);
    }

private:
    std::unordered_set<vertex_descriptor> command_vertices_;

    build::GraphPtr ptr_;
};

} }

#endif

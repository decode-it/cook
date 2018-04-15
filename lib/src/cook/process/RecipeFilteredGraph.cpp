#include "cook/process/RecipeFilteredGraph.hpp"

namespace cook { namespace process {

RecipeFilteredGraph::RecipeFilteredGraph(build::GraphPtr ptr)
    : ptr_(ptr)
{

}

RecipeFilteredGraph::vertex_descriptor RecipeFilteredGraph::goc_vertex(const FileLabel & path)
{
    return ptr_->goc_vertex(path);
}

RecipeFilteredGraph::vertex_descriptor RecipeFilteredGraph::add_vertex(CommandLabel ptr)
{
    vertex_descriptor vd = ptr_->add_vertex(ptr);
    command_vertices_.insert(vd);

    return vd;
}

Result RecipeFilteredGraph::add_edge(vertex_descriptor consumer, vertex_descriptor producer)
{
    return ptr_->add_edge(consumer, producer);
}

Result RecipeFilteredGraph::topological_commands(OrderedVertices & commands) const
{
    MSS_BEGIN(Result);

    OrderedVertices cmds;
    MSS(ptr_->topological_commands(cmds));
    L(C(cmds.size()));

    for(auto vd : cmds)
        if(command_vertices_.find(vd) != command_vertices_.end())
            commands.push_back(vd);

    MSS_END();
}

const RecipeFilteredGraph::Label & RecipeFilteredGraph::operator[](vertex_descriptor vd) const
{
    return ptr_->operator [](vd);
}

gubg::Range<RecipeFilteredGraph::CommandVertexIterator> RecipeFilteredGraph::command_vertices() const
{
    return gubg::make_range(command_vertices_);
}



} }


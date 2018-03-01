#include "cook/build/RecipeLocalGraph.hpp"

namespace cook { namespace build {

RecipeLocalGraph::RecipeLocalGraph(GraphPtr ptr)
    : ptr_(ptr)
{

}

RecipeLocalGraph::vertex_descriptor RecipeLocalGraph::goc_vertex(const FileLabel & path)
{
    return ptr_->goc_vertex(path);
}

RecipeLocalGraph::vertex_descriptor RecipeLocalGraph::add_vertex(CommandLabel ptr)
{
    vertex_descriptor vd = ptr_->add_vertex(ptr);
    command_vertices_.insert(vd);

    return vd;
}

Result RecipeLocalGraph::add_edge(vertex_descriptor src, vertex_descriptor tgt)
{
    return ptr_->add_edge(src, tgt);
}

Result RecipeLocalGraph::topological_commands(std::vector<vertex_descriptor> & commands) const
{
    MSS_BEGIN(Result);

    std::vector<vertex_descriptor> cmds;
    MSS(ptr_->topological_commands(cmds));

    for(auto vd : cmds)
        if(command_vertices_.find(vd) != command_vertices_.end())
            commands.push_back(vd);

    MSS_END();
}

const RecipeLocalGraph::Label & RecipeLocalGraph::operator[](vertex_descriptor vd) const
{
    return ptr_->operator [](vd);
}

gubg::Range<RecipeLocalGraph::CommandVertexIterator> RecipeLocalGraph::command_vertices() const
{
    return gubg::make_range(command_vertices_);
}

}

}


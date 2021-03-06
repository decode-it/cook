#include "cook/generator/graphviz/Component.hpp"
#include "cook/Context.hpp"

namespace cook { namespace generator { namespace graphviz {

std::string Component::name() const
{
    return "graphviz.component";
}

Result Component::set_option(const std::string & option)
{
    MSS_BEGIN(Result);
    set_filename(option);
    MSS_END();
}

bool Component::can_process(const Context & context) const
{
    return context.menu().is_valid();
}

Result Component::process(const Context & context)
{
    MSS_BEGIN(Result);

    std::ofstream ofs;
    MSS(open_output_stream(context, ofs));

    const auto & comp_g = context.menu().component_graph().graph;

    ofs << "digraph G {" << std::endl;
    ofs << "  compound=true;" << std::endl;

    for(auto v : gubg::graph::vertices(comp_g))
    {
        const auto & recipes = gubg::graph::vertex_label(v, comp_g);

        ofs << "  subgraph cluster_" << v << " {" << std::endl;
        for(model::Recipe * recipe : recipes)
        {
            // get the vertex index in the dependency graph
            auto it = context.menu().dependency_graph().translation_map.find(recipe);
            MSS(it != context.menu().dependency_graph().translation_map.end());

            ofs << "    " << it->second << " [label=\"" << recipe->uri() << "\"];" << std::endl;
        }
        ofs << "    label=\"\";" << std::endl;
        ofs << "    color=grey;" << std::endl;

        ofs << "  }" << std::endl;
    }

    const auto & dep_g = context.menu().dependency_graph().graph;
    for (auto e : gubg::graph::edges(dep_g))
        ofs << "  " << gubg::graph::source(e, dep_g) << " -> " << gubg::graph::target(e, dep_g) << ";" << std::endl;

    ofs << "}" << std::endl;

    MSS_END();
}

std::string Component::default_filename() const
{
    return "component.graphviz";
}


} } }

#include "cook/generator/Ninja.hpp"
#include "cook/Context.hpp"
#include "cook/log/Scope.hpp"

namespace cook { namespace generator { 

    Result Ninja::set_option(const std::string & option)
    {
        MSS_BEGIN(Result);
        set_filename(option);
        MSS_END();
    }

    bool Ninja::can_process(const Context & context) const
    {
        return context.menu().is_valid();
    }

    Result Ninja::process(std::ostream & ofs, const Context & context)
    {
        using vertex_descriptor = process::build::config::Graph::vertex_descriptor;

        MSS_BEGIN(Result);
        auto scope = log::Scope::top->scope("process");
        for (auto recipe: context.menu().topological_order_recipes())
        {
            recipe->stream(scope);

            auto build_graph_ptr = context.menu().recipe_filtered_graph(recipe);
            L(C(build_graph_ptr));
            MSS(!!build_graph_ptr);
            auto &build_graph = *build_graph_ptr;
            std::vector<vertex_descriptor> commands;
            MSS(build_graph.topological_commands(commands));

            auto scop = scope.scope("commands");
            scop.attr("size", commands.size());

            for (auto vertex: commands)
            {
                auto sco = scop.scope("vertex");
                const auto &label = build_graph[vertex];
                auto command_ptr = std::get_if<process::build::CommandPtr>(&label);
                /* auto command_ptr = std::get_if<std::filesystem::path>(&label); */
                MSS(!!command_ptr);
                auto &command = *command_ptr;
                sco.attr("name", command->name());
            }
        }
        MSS_END();
    }

    std::filesystem::path Ninja::output_filename(const model::Dirs & dirs) const
    {
        return dirs.output() / "build.ninja";
    }

} } 

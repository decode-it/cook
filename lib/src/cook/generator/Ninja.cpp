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

        ofs << "rule compile" << std::endl;
        ofs << "    command = g++ -std=c++17 -c $in -o $out" << std::endl;
        ofs << "rule archive" << std::endl;
        ofs << "    command = ar crf $out $in" << std::endl;
        ofs << "rule link" << std::endl;
        ofs << "    command = g++ -std=c++17 -o $out $in" << std::endl;

        for (auto recipe: context.menu().topological_order_recipes())
        {
            recipe->stream(scope);

            auto build_graph_ptr = context.menu().recipe_filtered_graph(recipe);
            L(C(build_graph_ptr));
            MSS(!!build_graph_ptr);
            auto &build_graph = *build_graph_ptr;
            process::RecipeFilteredGraph::OrderedVertices commands;
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
                process::RecipeFilteredGraph::Vertices inputs, outputs;
                build_graph.input_output(inputs, outputs, vertex);
                {
                    auto sc = sco.scope("inputs");
                    for (auto v: inputs)
                        sc.attr("file", std::get<process::build::config::Graph::FileLabel>(build_graph[v]));
                }
                {
                    auto sc = sco.scope("outputs");
                    for (auto v: outputs)
                        sc.attr("file", std::get<process::build::config::Graph::FileLabel>(build_graph[v]));
                }

                ofs << "build";
                for (auto v: outputs)
                    ofs << " " << std::get<process::build::config::Graph::FileLabel>(build_graph[v]).string();
                ofs << ": " << command->name();
                for (auto v: inputs)
                    ofs << " " << std::get<process::build::config::Graph::FileLabel>(build_graph[v]).string();
                ofs << std::endl;
            }
        }
        MSS_END();
    }

    std::filesystem::path Ninja::output_filename(const model::Dirs & dirs) const
    {
        return dirs.output() / "build.ninja";
    }

} } 

#include "cook/generator/Ninja.hpp"
#include "cook/Context.hpp"
#include "cook/log/Scope.hpp"
#include "boost/iterator/function_output_iterator.hpp"

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
        auto ss = log::scope("process");

        //We use only one simple rule, basically deferring the actual command to $process_command, defined
        //for each build seperately.
        //Trying to create rules per command that include $in or $out will be much more difficult to generalize.
        ofs << "rule process" << std::endl;
        ofs << "    command = $process_command" << std::endl;

        for (auto recipe: context.menu().topological_order_recipes())
        {
            recipe->stream();

            auto build_graph_ptr = context.menu().recipe_filtered_graph(recipe);
            L(C(build_graph_ptr));
            MSS(!!build_graph_ptr);
            auto &build_graph = *build_graph_ptr;
            process::RecipeFilteredGraph::OrderedVertices commands;
            MSS(build_graph.topological_commands(commands));

            auto ss = log::scope("commands", [&](auto & node) {
                node.attr("size", commands.size());
            });

            for (auto vertex: commands)
            {

                const auto &label = build_graph[vertex];
                auto command_ptr = std::get_if<process::build::Graph::CommandLabel>(&label);
                MSS(!!command_ptr);
                auto &command = *command_ptr;

                auto ss = log::scope("vertex", [&](auto & n) {
                    n.attr("name", command->name());
                });

                std::list<std::filesystem::path> input_files, output_files;
                auto input_it = boost::make_function_output_iterator([&](const auto & v) { input_files.push_back(std::get<process::build::config::Graph::FileLabel>(build_graph[v])); });
                auto output_it = boost::make_function_output_iterator([&](const auto & v) { output_files.push_back(std::get<process::build::config::Graph::FileLabel>(build_graph[v])); });

                build_graph.input_output(input_it, output_it, vertex);

                {
                    auto ss = log::scope("inputs", [&](auto & n) {
                        for (const auto & f: input_files)
                            n.attr("file", f);
                    });
                }
                {
                    auto ss = log::scope("outputs", [&](auto & n) {
                        for (const auto & f: output_files)
                            n.attr("file", f);
                    });

                }

                //Lambdas that stream the input and output files, prepended with a space.
                auto stream_inputs = [&](){
                    for (const auto & f: input_files)
                        ofs << " " << f.string();
                };
                auto stream_outputs = [&](){
                    for (const auto & f: output_files)
                        ofs << " " << f.string();
                };

                const auto cmd = command->name();

                //The build basically specifies the dependency between the output and input files
                ofs << "build";
                stream_outputs();
                ofs << ": process";
                stream_inputs();
                ofs << std::endl;

                //For each build, we have to specify the full $process_command
                ofs << "    process_command = ";

                if (false) {}
                else if (cmd == "archive")
                {
                    ofs << "ar crf";
                    stream_outputs();
                    stream_inputs();
                }
                else if (cmd == "link")
                {
                    ofs << "g++ -std=c++17 -o";
                    stream_outputs();
                    stream_inputs();
                }
                else
                {
                    command->to_stream(ofs, input_files, output_files);
                }
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

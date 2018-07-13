#include "cook/generator/Ninja.hpp"
#include "cook/process/toolchain/Types.hpp"
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

Result Ninja::process(const Context & context)
{
    MSS_BEGIN(Result);
    auto ss = log::scope("process");

    std::ofstream ofs;
    MSS(open_output_stream(context, ofs));

    std::map<cook::process::command::Ptr, std::string> command_map;
    auto goc_command = [&](cook::process::command::Ptr ptr)
    {
        auto it = command_map.find(ptr);
        if (it == command_map.end())
        {
            // generate the name
            std::string name = gubg::stream([&](auto & os) { os << ptr->type() << "_" << command_map.size(); });

            // add to them map
            it = command_map.insert(std::make_pair(ptr, name)).first;

            // and write out the command
            ofs << std::endl;
            ofs << "#Parent recipe: " << ptr->recipe_uri() << std::endl;
            const process::command::Filenames input = { "${in}" };
            const process::command::Filenames output = { "${out}" };
            ptr->set_inputs_outputs(input, output);
            bool has_deps = false;
            {
                std::ostringstream oss;
                //See if this command contains a non-empty deps part
                ptr->stream_part(oss, process::toolchain::Part::Deps);
                if (!oss.str().empty())
                {
                    has_deps = true;
                    ofs << "msvc_deps_prefix = Note: including file:" << std::endl;
                }
            }
            ofs << "rule " << name << std::endl;
            {
                //Identity translator, used to get the kv.first directly
                process::toolchain::Translator trans = [](const std::string &k, const std::string &v){return k;};

                std::ostringstream oss;
                if (has_deps)
                {
                    oss.str("");
                    ptr->stream_part(oss, process::toolchain::Part::Deps, &trans);
                    ofs << "   deps = " << oss.str() << std::endl;
                }
                //See if this command contains a non-empty depfile part
                oss.str("");
                ptr->stream_part(oss, process::toolchain::Part::DepFile);
                if (!oss.str().empty())
                {
                    //Extract the dependency file without the toolchain-specific arguments around it: we only want the filename
                    oss.str("");
                    ptr->stream_part(oss, process::toolchain::Part::DepFile, &trans);
                    const auto depfile = oss.str();
                    if (!depfile.empty())
                        ofs << "   depfile = " << depfile << std::endl;
                }
            }
            ofs << "   command = ";
            ptr->stream_command(ofs);
            ofs << std::endl;
        }

        return it->second;
    };

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

            auto ss = log::scope("vertex", [&](auto & n) { n.attr("name", command->name()); });

            std::list<std::filesystem::path> input_files;
            {
                auto func = [&](const auto & v)
                {
                    input_files.push_back(std::get<process::build::config::Graph::FileLabel>(build_graph[v]));
                };
                build_graph.input(func, vertex, cook::process::RecipeFilteredGraph::Explicit);

                auto ss = log::scope("inputs", [&](auto & n) {
                    for (const auto & f: input_files)
                        n.attr("file", f);
                });
            }

            std::list<std::filesystem::path> input_dependencies;
            {
                auto func = [&](const auto & v)
                {
                    input_dependencies.push_back(std::get<process::build::config::Graph::FileLabel>(build_graph[v]));
                };
                build_graph.input(func, vertex, cook::process::RecipeFilteredGraph::Implicit);

                auto ss = log::scope("implicit inputs", [&](auto & n) {
                    for (const auto & f: input_dependencies)
                        n.attr("file", f);
                });
            }

            std::list<std::filesystem::path> output_files;
            {
                auto func = [&](const auto & v)
                {
                    output_files.push_back(std::get<process::build::config::Graph::FileLabel>(build_graph[v]));
                };
                build_graph.output(func, vertex);

                auto ss = log::scope("outputs", [&](auto & n) {
                    for (const auto & f: output_files)
                        n.attr("file", f);
                });
            }

            const auto build_command = goc_command(command);
            //The build basically specifies the dependency between the output and input files
            ofs << "build";
            auto stream_escaped = [&](const std::string &str){
                for (const auto ch: str)
                {
                    if (ch == ':')
                        ofs << '$';
                    ofs << ch;
                }
            };
            for (const auto & f: output_files)
            {
                ofs << " ";
                stream_escaped(f.string());
            }
            ofs << ": ";
            stream_escaped(build_command);
            for (const auto & f: input_files)
            {
                ofs << " ";
                stream_escaped(f.string());
            }
            ofs << " |";
            for (const auto & f: input_dependencies)
            {
                ofs << " ";
                stream_escaped(f.string());
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

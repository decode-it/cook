#include "cook/generator/Ninja.hpp"
#include "cook/process/toolchain/Types.hpp"
#include "cook/Context.hpp"
#include "cook/log/Scope.hpp"
#include "cook/OS.hpp"

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

    namespace {
        std::string escape_ninja(const std::string & str, bool quote_spaces)
        {
            std::string res;

            bool has_spaces = true;
            for (const auto ch: str)
            {
                has_spaces = has_spaces || ch == ' ';
                if (ch == ':' || ch == ' ')
                    res += '$';
                res += ch;
            }

            if(has_spaces && quote_spaces)
                res = std::string("\"") + res  + std::string("\"");
            return res;
        }

        void stream_command(std::ostream & ofs, cook::process::command::Ptr ptr)
        {
            ofs << "   command = ";

            bool add_quotes = false;
            auto escape = [&add_quotes](process::toolchain::Part p, const std::string & str)
            {
                return escape_ninja(str, add_quotes);
            };

            if (ptr->delete_before_build())
            {

                switch(get_os())
                {
                    case OS::MacOS:
                    case OS::Linux:
                        ofs << "rm -f \"${out}\" && ";
                        ptr->stream_command(ofs, escape);
                        break;
                        
                    case OS::Windows:
                        add_quotes = true;
                         ofs << "cmd /c \"del \"${out}\" && ";
                         ptr->stream_command(ofs, escape);
                         ofs << "\"";
                         break;
                         
                    default:
                        ptr->stream_command(ofs, escape);
                        break;
                }
            }
            else
            {
                ptr->stream_command(ofs, escape);
            }


            ofs << std::endl;

        }
    }

    Result Ninja::process(const Context & context)
    {
        MSS_BEGIN(Result);
        auto ss = log::scope("process");

        std::ofstream ofs;
        MSS(open_output_stream(context, ofs));

        std::unique_ptr<std::ofstream> response_file;

        std::map<std::string, unsigned int> uri_count_map;
        std::map<cook::process::command::Ptr, std::string> command_map;
        auto goc_command = [&](cook::process::command::Ptr ptr, const std::string & uri, std::string & cmd_name)
        {
            MSS_BEGIN(Result);

            // do we have this command
            auto it = command_map.find(ptr);
            if (it != command_map.end())
            {
                cmd_name = it->second;
                MSS_RETURN_OK();
            }

            unsigned int count = uri_count_map[uri]++;

            // generate the name
            cmd_name = gubg::stream([&](auto & os) { os << uri << "_" << count; });
            bool add_to_map = true;

            process::command::Filenames input = { "${in}" };
            process::command::Filenames output = { "${out}" };

            // check whether we want a response file
            {
                response_file.reset();
                std::ostringstream oss;
                oss << cmd_name + ".resp";
                std::filesystem::path response_filename = context.dirs().output() / oss.str();
                std::string resp = ptr->get_kv_part(process::toolchain::Part::Response, response_filename.string());

                if (!resp.empty())
                {
                    input = { resp };

                    response_file = std::make_unique<std::ofstream>(resp);
                    MSS(util::open_file(response_filename, *response_file));

                    add_to_map = false;
                }
            }

            // and write out the command
            ofs << std::endl;
            ofs << "#Parent recipe: " << ptr->recipe_uri() << std::endl;
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
            ofs << "rule " << cmd_name << std::endl;
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

            stream_command(ofs, ptr);

            // add to them map
            if (add_to_map)
                command_map[ptr] = cmd_name;

            MSS_END();
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

                std::string build_command;
                MSS(goc_command(command, recipe->uri().string(false, '_'), build_command));
                //The build basically specifies the dependency between the output and input files
                ofs << "build";
                auto stream_escaped = [&](const std::string &str) {
                    ofs << escape_ninja(str, false);
                };
                for (const auto & f: output_files)
                {
                    ofs << " ";
                    stream_escaped(f.string());
                }
                ofs << ": ";
                stream_escaped(build_command);
                //Emile: This is a workaround for 1 specific problem on windows:
                //To build with mfc properly, the translation units (.obj) containing
                //stdafx.h need to be linked first, otherwise we have a
                //duplicate dllmain() definition.
                //I noticed that the recipe's own object files were always
                //last, so I reversed the order here.
                for (auto f = input_files.rbegin(); f != input_files.rend(); ++f)
                {
                    ofs << " ";
                    stream_escaped(f->string());

                    if (response_file)
                        (*response_file) << "\"" << f->string() << "\"" << std::endl;
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

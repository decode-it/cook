#include "cook/work/NinjaWriter.hpp"
#include "cook/work/TopologicalOrder.hpp"
#include <ctime>
#include <iomanip>

namespace cook { namespace work { 

    bool NinjaWriter::operator()(std::ostream & os, const Recipes & recipes, const structure::Uri & default_uri)
    {
        MSS_BEGIN(bool);

        MSS(write_header_(os));

        os << "#>> Global variables" << std::endl;
        MSS(write_global_vars_(os));
        os << "#<< Global variables" << std::endl << std::endl;

        os << "#>> Rule definitions" << std::endl;
        MSS(write_rules_(os));
        os << "#<< Rule definitions" << std::endl << std::endl;

        os << "#>> Clean build statement" <<std::endl;
        os << "build clean: clean" << std::endl;       
        os << "#<< Clean build statement" <<std::endl << std::endl;

        os << "#>> Recipes" << std::endl;
        {
            //TODO: More subtle aggregation of defines and include paths depending on public/private
            std::set<std::string> defines;
            std::set<std::string> include_paths;
            std::set<std::string> force_includes;
            for (auto * recipe : recipes)
            {
                for(const auto & p: recipe->defines())
                {
                    std::string def = p.first;
                    if (!p.second.empty())
                    {
                        def += "=";
                        def += p.second;
                    }
                    defines.insert(def);
                }

                for (const auto &i: recipe->include_paths())
                    include_paths.insert(i);

                for (const auto &p: recipe->force_includes())
                    force_includes.insert(p.first);
            }
            os << "defines =";
            for (const auto &def: defines)
                os << " -D" << def;
            os << std::endl;
            os << "include_paths =";
            for (const auto &inc: include_paths)
                os << " -I " << inc;
            os << std::endl;
            os << "force_includes =";
            for (const auto &p: force_includes)
                os << " -include " << p;
            os << std::endl;
        }
        for (auto * recipe : recipes)
        {
            os << "#  >> Recipe \"" << recipe->uri().string() << "\"" << std::endl;
            MSS(write_recipe_(os, *recipe, recipes));
            os << "#  << Recipe \"" << recipe->uri().string() << "\"" << std::endl << std::endl;
        }
        os << "#<< Recipes" <<std::endl << std::endl;

        os << "#>> Default build statement" <<std::endl;
        os << "default recipe_" << default_uri << std::endl;
        os << "#<< Default build statement" << std::endl << std::endl;

        MSS_END();
    }

    bool NinjaWriter::write_header_(std::ostream & os)
    {
        MSS_BEGIN(bool);
        //Version and time
        //TODO: write actual version
            std::time_t t = std::time(nullptr);
            os << "#File written by cook (1.0.0) on " << std::put_time(std::gmtime(&t), "%c") << std::endl;
            os << std::endl;
        MSS_END();
    }

    bool NinjaWriter::write_global_vars_(std::ostream & os)
    {
        MSS_BEGIN(bool);

        Toolchain::CompileInfo compile;
        Toolchain::LinkInfo link;
        Toolchain::ArchiveInfo archive;
        MSS(toolchain.get_info(compile, link, archive));

        os << "builddir = " << options.build_dir.native() << std::endl;

        os << "compiler = " << compile.cmd << std::endl;
        os << "cflags = " << compile.flags << compile.defines << std::endl;

        os << "linker = " << link.cmd << std::endl;
        os << "lflags = " << link.flags << std::endl;

        os << "archiver = " << archive.cmd << std::endl;
        os << "aflags = " << archive.flags << std::endl;

        MSS_END();
    }

    bool NinjaWriter::write_rules_(std::ostream & os)
    {
        MSS_BEGIN(bool);

        os
        << "rule compile" << std::endl
        << "  command = $compiler -c -MD -MF $out.d $cflags $defines -o $out $in $include_paths $force_includes" << std::endl
        << "  depfile = $out.d" << std::endl;

        os
        << "rule link" << std::endl
        << "  command = $linker $lflags -o $out $in $library_paths $libraries" << std::endl;

        os
        << "rule archive" << std::endl
        << "  command = $archiver $out $in" << std::endl;

        os
        << "rule clean" << std::endl
        << "  command = ninja -C $builddir -t clean" << std::endl;

        MSS_END();
    }

    bool NinjaWriter::write_recipe_(std::ostream & os, const Recipe & recipe, const Recipes & order)
    {
        MSS_BEGIN(bool);

        const auto & info = recipe.output();

        // the settings for this recipe
        auto obj_tgt = [&](const Recipe &r)
        {
            std::filesystem::path res = options.build_dir / ".cook" / to_path(r.uri());
            toolchain.add_highlights(res);
            return res;
        };

        auto make_absolute = [&](const std::filesystem::path & p)
        {
            return (p.is_absolute() ? p : options.build_dir / p);
        };

        os
        << "#     target type: " << recipe.target_type() << std::endl
        << "#     temporary files: " << obj_tgt(recipe).string() << std::endl
        << "#     result: " << info.filename << std::endl;

        // extra material, coming from the dependencies
        structure::TargetConfig cfg;
        cfg = recipe.input_config();

        {
            std::list<structure::Recipe *> suborder;
            subset_order(std::back_inserter(suborder), recipe.uri(), util::make_range(order));

            for(auto it = suborder.rbegin(); it != suborder.rend(); ++it)
            {
                auto * dep = *it;
                structure::merge(cfg, dep->output());
            }
        }


        // the library paths
        {
            os << "library_paths =";
            for (const auto & l: cfg.library_paths)
                os << " -L" << make_absolute(l).string();
            os << std::endl;
        }

        // the libraries
        {
            os << "libraries =";
            for (const auto & l: cfg.libraries)
                os << " -l" << l;
            os << std::endl;
        }

        // the build files
        std::list<std::filesystem::path> objects;
        for (const auto &p: recipe.sources())
        {
            const auto &source = p.first;
            std::filesystem::path obj = obj_tgt(recipe) / source; obj += ".obj";
            objects.push_back(obj);
            os << "build " << obj.string() << ": compile " << source.string() << std::endl;            
        }


        // the linking
        {
            std::string command;
            switch (recipe.target_type())
            {
                case structure::TargetType::Executable:     command = "link"; break;
                case structure::TargetType::StaticLibrary:  command = "archive"; break;
                default: break;
            }

            if (!command.empty())
            {
                os << "build " << info.filename.native() << ":  " << command;
                for (const auto & obj: objects)
                    os << " $" << std::endl << "    " << obj.string();

                if (!recipe.required_recipes().empty())
                {
                    std::vector<const Recipe *> implicit_deps;

                    for (auto *dep : recipe.required_recipes())
                    {
                        switch (dep->target_type())
                        {
                            case structure::TargetType::Executable:
                            case structure::TargetType::StaticLibrary:
                                implicit_deps.push_back(dep);
                                break;
                            default:
                                for (const auto &p: dep->sources())
                                {
                                    const auto &source = p.first;
                                    std::filesystem::path obj = obj_tgt(*dep) / source; obj += ".obj";
                                    os << " $" << std::endl << "    " << obj.string();
                                }
                                break;
                        }
                    }

                    if (!implicit_deps.empty())
                    {
                        os << " |";
                        for (auto dep: implicit_deps)
                            os << " recipe_" << dep->uri();
                    }
                }
                os << std::endl;

                os << "build recipe_" << recipe.uri() << ": phony " << info.filename.native() << std::endl;
                os << std::endl << std::endl;
            }
        }

        MSS_END();
    }
} }

#ifndef HEADER_cook_presenter_NinjaWriter_hpp_ALREADY_INCLUDED
#define HEADER_cook_presenter_NinjaWriter_hpp_ALREADY_INCLUDED

#include "cook/model/toolchain/Toolchain.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/Types.hpp"
#include "gubg/std/filesystem.hpp"
#include "gubg/string_algo/substitute.hpp"
#include "gubg/mss.hpp"
#include <ostream>

namespace cook { namespace presenter { 

    class NinjaWriter
    {
    public:
        NinjaWriter(std::ostream &os): os_(os) { }

        bool write(const model::Env &env, const model::toolchain::Toolchain &toolchain, const model::RecipeDAG &dag)
        {
            MSS_BEGIN(bool);
            MSS(write_(env));
            MSS(write_(toolchain));
            MSS(write_(dag));
            MSS_END();
        }

    private:
        bool write_(const model::Env &env)
        {
            MSS_BEGIN(bool);
            os_ << std::endl;
            os_ << "# >> Environment" << std::endl;
            os_ << "builddir = " << env.dir("build").string() << std::endl;
            os_ << "# << Environment" << std::endl;
            MSS_END();
        }
        bool write_(const model::toolchain::Toolchain &toolchain)
        {
            MSS_BEGIN(bool);
            os_ << std::endl;
            os_ << "# >> Toolchain" << std::endl;
            MSS(toolchain.get_info(compiler_, linker_, archiver_));

            //Compiler rules and global settings
            {
                model::toolchain::Compiler::TemplateStubs cstubs;
                {
                    cstubs.object = "$out";
                    cstubs.source = "$in";
                    cstubs.depfile = "$out.d";
                    cstubs.flags = "$cflags";
                    cstubs.defines = "$defines";
                    cstubs.include_paths = "$include_paths";
                    cstubs.force_includes = "$force_includes";
                }

                os_
                << "rule compile_c" << std::endl
                << "  command = " << compiler_->cmd_template("c", cstubs) << std::endl
                << "  depfile = $out.d" << std::endl;

                os_
                << "rule compile_cpp" << std::endl
                << "  command = " << compiler_->cmd_template("c++", cstubs) << std::endl
                << "  depfile = $out.d" << std::endl;

                os_
                << "rule compile_asm" << std::endl
                << "  command = " << compiler_->cmd_template("asm", cstubs) << std::endl
                << "  depfile = $out.d" << std::endl;

                model::toolchain::Flags flags;
                os_ << "cflags = " << compiler_->prepare_flags(flags) << std::endl;


                os_ << std::endl;
            }

            //Linker rules and global settings
            {
                model::toolchain::Linker::TemplateStubs lstubs;
                {
                    lstubs.executable = "$out";
                    lstubs.objects = "$in";
                    lstubs.flags = "$lflags";
                    lstubs.library_paths = "$library_paths";
                    lstubs.libraries = "$libraries";
                }

                os_
                << "rule link" << std::endl
                << "  command = " << linker_->cmd_template(lstubs) << std::endl;

                model::toolchain::Flags flags;
                os_ << "lflags = " << linker_->prepare_flags(flags) << std::endl;

                os_ << std::endl;
            }

            //Archiver rules and global settings
            {
                model::toolchain::Archiver::TemplateStubs astubs;
                {
                    astubs.library = "$out";
                    astubs.objects = "$in";
                    astubs.flags = "$aflags";
                }

                os_
                << "rule archive" << std::endl
                << "  command = " << archiver_->cmd_template(astubs) << std::endl;

                model::toolchain::Flags flags;
                os_ << "aflags = " << archiver_->prepare_flags(flags) << std::endl;

                os_ << std::endl;
            }

            os_ << "# << Toolchain" << std::endl;
            MSS_END();
        }
        static std::string escape_(const std::string &orig)
        {
            std::string esc;
            gubg::string_algo::substitute<std::string>(esc, orig, ":", "$:");
            return esc;
        }
        bool write_(const model::RecipeDAG &dag)
        {
            MSS_BEGIN(bool);
            std::ostringstream oss;
            auto object_fn = [&](const auto &file){
                oss.str("");
                oss << "${builddir}" << std::filesystem::path::preferred_separator << file.path.relative_path().string() << ".obj";
                return escape_(oss.str());
            };
            auto compile_rule = [](const auto &file){
                if (false) {}
                else if (file.language == "c") return "compile_c";
                else if (file.language == "c++") return "compile_cpp";
                else if (file.language == "asm") return "compile_asm";
                return "";
            };
            auto source_fn = [](const auto &file){
                return escape_(file.path.string());
            };
            auto local_name = [&](const model::Recipe &r, const char *name){
                oss.str("");
                oss << r.uri().str('\0', '_') << "_" << name;
                return oss.str();
            };
            auto write_recipe = [&](const model::Recipe &recipe){
                MSS_BEGIN(bool);
                os_ << std::endl;
                os_ << "# >> Recipe " << recipe.uri_hr() << std::endl;
#if 0
                os_ << local_name(recipe, "include_paths") << " = " << compiler_->prepare_include_paths(recipe.include_paths(model::Owner::Anybody)) << std::endl;
#endif
                auto write_compile = [&](const auto &file){
                    if (file.type == FileType::Source)
                    {
                        const auto obj_fn = object_fn(file);
                        os_ << "build " << obj_fn << ": " << compile_rule(file) << " " << source_fn(file) << std::endl;
#if 1
                        os_ << "    include_paths = " << compiler_->prepare_include_paths(recipe.include_paths(model::Owner::Anybody)) << std::endl;
#else
                        os_ << "    include_paths = $" << local_name(recipe, "include_paths");
                        auto add_ip_for_deps = [&](const model::Recipe &d){
                            os_ << " $" << local_name(d, "include_paths");
                            return true;
                        };
                        dag.each_out(&recipe, add_ip_for_deps);
                        os_ << std::endl;
#endif
                        os_ << "    force_includes = " << compiler_->prepare_force_includes(recipe.force_includes()) << std::endl;
                        os_ << "    library_paths = " << std::endl;
                        os_ << "    libraries = " << std::endl;

                        model::toolchain::Defines defines;
                        for(const auto & p : recipe.defines())
                            defines.push_back(compiler_->create_define(p.first, p.second));
                        os_ << "    defines = " << compiler_->prepare_defines(defines) << std::endl;
                    }
                    return true;
                };
                MSS(recipe.each_file(write_compile, model::Owner::Me));
                if (false) {}
                else if (recipe.type() == "executable")
                {
                    os_ << "build " << recipe.output().filename.string() << ": link ";
                    auto add_object = [&](const auto &f){
                        if (f.type == FileType::Source)
                            os_ << "$\n    " << object_fn(f) << " ";
                        return true;
                    };
                    MSS(recipe.each_file(add_object, model::Owner::Anybody));
                    {
                        gubg::OnlyOnce add_pipe;
                        for (const auto &lib: recipe.libraries(model::Owner::Me | model::Owner::Deps))
                        {
                            if (add_pipe())
                                os_ << " |";
                            os_ << " " << archiver_->prepare_library(lib);
                        }
                    }
                    os_ << std::endl;
                    os_ << "    library_paths = " << linker_->prepare_library_paths(recipe.library_paths());
                    os_ << std::endl;
                    os_ << "    libraries = " << linker_->prepare_libraries(recipe.libraries(model::Owner::Anybody));
                    os_ << std::endl;
                }
                else if (recipe.type() == "library")
                {
                    os_ << "build " << archiver_->prepare_library(recipe.output().name) << ": archive ";
                    auto add_object = [&](const auto &f){
                        if (f.type == FileType::Source)
                            os_ << "$\n    " << object_fn(f) << " ";
                        return true;
                    };
                    MSS(recipe.each_file(add_object, model::Owner::Anybody));
                    os_ << std::endl;
                }
                os_ << "# << Recipe " << recipe.uri_hr() << std::endl;
                MSS_END();
            };
            dag.each_vertex<gubg::network::Direction::Backward>(write_recipe);
            MSS_END();
        }

        std::ostream &os_;
        model::toolchain::Compiler::Ptr compiler_;
        model::toolchain::Linker::Ptr linker_;
        model::toolchain::Archiver::Ptr archiver_;
    };

} } 

#endif

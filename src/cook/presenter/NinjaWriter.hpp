#ifndef HEADER_cook_presenter_NinjaWriter_hpp_ALREADY_INCLUDED
#define HEADER_cook_presenter_NinjaWriter_hpp_ALREADY_INCLUDED

#include "cook/model/Toolchain.hpp"
#include "gubg/std/filesystem.hpp"
#include "gubg/mss.hpp"
#include <fstream>

namespace cook { namespace presenter { 

    class NinjaWriter
    {
    public:
        NinjaWriter(const std::string &fn): fo_(fn)
        {
        }

        bool write(const model::Env &env, const model::Toolchain &toolchain, const model::RecipeDAG &dag)
        {
            MSS_BEGIN(bool);
            MSS(write_(env));
            MSS(write_(toolchain));
            MSS(write_rules_());
            MSS(write_(dag));
            MSS_END();
        }

    private:
        bool write_(const model::Env &env)
        {
            MSS_BEGIN(bool);
            fo_ << std::endl;
            fo_ << "# >> Environment" << std::endl;
            fo_ << "builddir = " << env.build_dir().native() << std::endl;
            fo_ << "# << Environment" << std::endl;
            MSS_END();
        }
        bool write_(const model::Toolchain &toolchain)
        {
            MSS_BEGIN(bool);
            fo_ << std::endl;
            fo_ << "# >> Toolchain" << std::endl;
            model::Toolchain::CompileInfo compile;
            model::Toolchain::LinkInfo link;
            model::Toolchain::ArchiveInfo archive;
            MSS(toolchain.get_info(compile, link, archive));

            fo_ << "compiler_c = " << compile.cmd_c << std::endl;
            fo_ << "compiler_cpp = " << compile.cmd_cpp << std::endl;
            fo_ << "compiler_asm = " << compile.cmd_asm << std::endl;
            fo_ << "cflags = " << compile.flags << compile.defines << std::endl;

            fo_ << "linker = " << link.cmd << std::endl;
            fo_ << "lflags = " << link.flags << std::endl;

            fo_ << "archiver = " << archive.cmd << std::endl;
            fo_ << "aflags = " << archive.flags << std::endl;
            fo_ << "# << Toolchain" << std::endl;
            MSS_END();
        }
        bool write_rules_()
        {
            MSS_BEGIN(bool);
            fo_ << std::endl;
            fo_ << "# >> Rules" << std::endl;

            fo_
            << "rule compile_c" << std::endl
            << "  command = $compiler_c -c -MD -MF $out.d $cflags $defines -o $out $in $include_paths $force_includes" << std::endl
            << "  depfile = $out.d" << std::endl;

            fo_
            << "rule compile_cpp" << std::endl
            << "  command = $compiler_cpp -c -MD -MF $out.d $cflags $defines -o $out $in $include_paths $force_includes" << std::endl
            << "  depfile = $out.d" << std::endl;

            fo_
            << "rule compile_asm" << std::endl
            << "  command = $compiler_asm $in -o $out" << std::endl
            << "  depfile = $out.d" << std::endl;

            fo_
            << "rule link" << std::endl
            << "  command = $linker $lflags -o $out $in $library_paths $libraries" << std::endl;

            fo_
            << "rule archive" << std::endl
            << "  command = $archiver $aflags $out $in" << std::endl;

            fo_
            << "rule clean" << std::endl
            << "  command = ninja -C $builddir -t clean" << std::endl;

            fo_ << "# << Rules" << std::endl;
            MSS_END();
        }
        bool write_(const model::RecipeDAG &dag)
        {
            MSS_BEGIN(bool);
            auto write_recipe = [&](model::Recipe *recipe){
                fo_ << std::endl;
                fo_ << "# >> Recipe " << recipe->uri() << std::endl;
                fo_ << "# << Recipe " << recipe->uri() << std::endl;
                return true;
            };
            dag.each_vertex(write_recipe);
            MSS_END();
        }

        std::ofstream fo_;
    };

} } 

#endif

#include "cook/process/command/Toolchain.hpp"
#include "cook/process/command/gcclike/Compiler.hpp"
#include "cook/process/command/gcclike/Archiver.hpp"
#include "cook/process/command/gcclike/Linker.hpp"
#include "cook/log/Scope.hpp"
#include "cook/OS.hpp"

namespace cook { namespace process { namespace command { 

    bool Toolchain::set_brand(const std::string &brand)
    {
        MSS_BEGIN(bool);
        if (false) {}
        else if (brand == "gcc") {}
        else if (brand == "clang") {}
        else if (brand == "msvc") {}
        else { MSS(false); }
        brand_ = brand;
        MSS_END();
    }

    bool Toolchain::initialize()
    {
        MSS_BEGIN(bool);

        //Default brand based on OS
        if (brand_.empty())
            switch (get_os())
            {
                case OS::Linux: brand_ = "gcc"; break;
                case OS::Windows: brand_ = "msvc"; break;
                case OS::MacOS: brand_ = "clang"; break;
                default: brand_ = "gcc"; break;
            }

        MSS_END();
    }

    Result Toolchain::create_compiler(CompilerPtr &ptr, Language language) const
    {
        MSS_BEGIN(Result);
        auto ss = log::scope("create_compiler", [&](auto &node){node.attr("brand", brand_);});
        if (false) {}
        else if (brand_ == "gcc")   {ptr.reset(new command::gcclike::Compiler("gcc",   "g++",     language));}
        else if (brand_ == "clang") {ptr.reset(new command::gcclike::Compiler("clang", "clang++", language));}
        MSS(!!ptr);
        MSS_END();
    }
    Result Toolchain::create_archiver(ArchiverPtr &ptr) const
    {
        MSS_BEGIN(Result);
        auto ss = log::scope("create_archiver", [&](auto &node){node.attr("brand", brand_);});
        if (false) {}
        else if (brand_ == "gcc" || brand_ == "clang")
        {
            std::string args;
            switch (get_os())
            {
                case OS::Linux: args = "crf"; break;
                case OS::MacOS: args = "crs"; break;
                default: break;
            }
            ptr.reset(new command::gcclike::Archiver(args));
        }
        MSS(!!ptr);
        MSS_END();
    }
    Result Toolchain::create_linker(LinkerPtr &ptr) const
    {
        MSS_BEGIN(Result);
        auto ss = log::scope("create_linker", [&](auto &node){node.attr("brand", brand_);});
        if (false) {}
        else if (brand_ == "gcc")   {ptr.reset(new command::gcclike::Linker("g++"));}
        else if (brand_ == "clang") {ptr.reset(new command::gcclike::Linker("clang++"));}
        MSS(!!ptr);
        MSS_END();
    }

} } } 

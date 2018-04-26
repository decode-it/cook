#include "cook/process/command/Toolchain.hpp"
#include "cook/process/command/gcclike/Compiler.hpp"
#include "cook/log/Scope.hpp"
#include "gubg/platform/os_api.h"

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
        if (brand_.empty())
        {
            brand_ = "gcc";
#ifdef GUBG_API_LINUX
            brand_ = "gcc";
#endif
#ifdef GUBG_API_APPLE
            brand_ = "clang";
#endif
#ifdef GUBG_API_WIN32
            brand_ = "msvc";
#endif
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

} } } 

#ifndef HEADER_cook_process_toolchain_gcc_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_gcc_Compiler_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/gcc.hpp"
#include "cook/process/toolchain/Compiler.hpp"
#include "cook/Language.hpp"
#include "gubg/mss.hpp"
#include <optional>

namespace cook { namespace process { namespace toolchain { namespace gcc {

    inline bool set_compiler(Compiler & compiler, const std::string & executable)
    {
        MSS_BEGIN(bool);

        auto & trans = compiler.translator_map();
        auto & kvm = compiler.key_values_map();

        kvm[Part::Cli].emplace_back(executable, "");
        trans[Part::Cli] = [=](const std::string &k, const std::string &){return k; }; 

        trans[Part::Pre] = [](const std::string &k, const std::string &v)
        {
            if (v.empty())
                return k;
            return k+"="+v;
        };
        trans[Part::Output] = [](const std::string &k, const std::string &v){return std::string{"-o "}+k;};
        trans[Part::Input] = [](const std::string &k, const std::string &v){return k;};
        trans[Part::DepFile] = [](const std::string &k, const std::string &v){return std::string{"-MMD -MF "}+k;};
        trans[Part::Define] = [](const std::string &k, const std::string &v)
        {
            if (v.empty())
                return std::string{"-D"}+k;
            return std::string{"-D"}+k+"="+v;
        };
        trans[Part::IncludePath] = [](const std::string &k, const std::string &v){return std::string{"-I "}+(k.empty() ? "./" : k);};
        trans[Part::ForceInclude] = [](const std::string &k, const std::string &v){return std::string{"-include "}+k;};

        kvm[Part::Pre].emplace_back("-c", "");
        kvm[Part::Pre].emplace_back("-fPIC", "");

        const Language & language = compiler.language;
        
        auto configure = [&](const std::string &key, const std::string &value, const Configuration & conf)
        {
            if (false) {}
            else if (key == "debug_symbols" && value == "true")
                kvm[Part::Pre].emplace_back("-g", "");
            else if (key == "optimization" && value == "max_speed")
                kvm[Part::Pre].emplace_back("-O3", "");
            else if (key == "config" && value == "rtc")
            {
                kvm[Part::Pre].emplace_back("-fsanitize", "address");
                kvm[Part::Pre].emplace_back("-fno-omit-frame-pointer", "");
            }
            else if (key == "config" && value == "profile")
                kvm[Part::Pre].emplace_back("-g", "");
            else if (key == "arch" && value == "x86")
                kvm[Part::Pre].emplace_back("-m32", "");
            else if (key == "arch" && value == "x64")
                kvm[Part::Pre].emplace_back("-m64", "");
            else if (key == "arch" && value == "armv7")
            {
                kvm[Part::Cli].clear();
                kvm[Part::Cli].emplace_back("/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang", "");
                if (language == Language::CXX)
                {
                    kvm[Part::Pre].emplace_back("-x c++", "");
                    kvm[Part::Pre].emplace_back("-stdlib=libc++ -Wno-c++11-extensions", "");
                }
                kvm[Part::Pre].emplace_back("-arch armv7", "");
                kvm[Part::Pre].emplace_back("-fmessage-length=0 -fdiagnostics-show-note-include-stack -fmacro-backtrace-limit=0 -Wno-trigraphs -fpascal-strings -Wno-missing-field-initializers -Wno-missing-prototypes -Wno-return-type -Wno-non-virtual-dtor -Wno-overloaded-virtual -Wno-exit-time-destructors -Wno-missing-braces -Wparentheses -Wswitch -Wno-unused-function -Wno-unused-label -Wno-unused-parameter -Wno-unused-variable -Wunused-value -Wno-empty-body -Wno-uninitialized -Wno-unknown-pragmas -Wno-shadow -Wno-four-char-constants -Wno-conversion -Wno-constant-conversion -Wno-int-conversion -Wno-bool-conversion -Wno-enum-conversion -Wno-float-conversion -Wno-non-literal-null-conversion -Wno-objc-literal-conversion -Wno-shorten-64-to-32 -Wnewline-eof -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS11.3.sdk -fstrict-aliasing -Wdeprecated-declarations -Winvalid-offsetof -miphoneos-version-min=9.3 -g -Wno-sign-conversion -Wno-infinite-recursion -Wno-move -Wno-comma -Wno-block-capture-autoreleasing -Wno-strict-prototypes -Wno-range-loop-analysis -fembed-bitcode-marker -Wall -Wextra", "");
                kvm[Part::Define].emplace_back("MAC", "1");
            }
            else if (key == "c++-standard" && language == Language::CXX)
                kvm[Part::Pre].emplace_back("-std", "c++" + value);
            else if (key == "c-standard" && language == Language::C)
                kvm[Part::Pre].emplace_back("-std", "c" + value);
            else
            {
                return false;
            }
            return true;
        };

        compiler.add_config(100, configure);
        MSS_END();
    }

} } } }

#endif

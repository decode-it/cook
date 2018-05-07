#ifndef HEADER_cook_process_toolchain_msvc_Linker_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_msvc_Linker_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Linker.hpp"

namespace cook { namespace process { namespace toolchain { namespace msvc {

    inline bool set_linker(Linker & linker)
    {
        MSS_BEGIN(bool);

        auto & trans = linker.translator_map();
        trans[Part::Cli] = [](const std::string &, const std::string &){return "link";};
        trans[Part::Output] = [](const std::string &k, const std::string &v){return std::string{"/OUT:"}+k;};
        trans[Part::Input] = [](const std::string &k, const std::string &v){return k;};
        trans[Part::Library] = [](const std::string &k, const std::string &v){return k+".lib";};
        trans[Part::LibraryPath] = [](const std::string &k, const std::string &v){return std::string{"/LIBPATH:"}+(k.empty() ? "./" : k);};

        MSS_END();
    }

} } } }

#endif

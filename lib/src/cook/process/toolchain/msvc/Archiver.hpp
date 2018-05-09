#ifndef HEADER_cook_process_toolchain_msvc_Archiver_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_msvc_Archiver_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Archiver.hpp"
#include "cook/OS.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace process { namespace toolchain { namespace msvc {

    inline bool configure_archiver(Archiver & archiver)
    {
        MSS_BEGIN(bool);
        MSS(toolchain::configure_archiver(archiver));

        auto & trans = archiver.translator_map();
        trans[Part::Cli] = [](const std::string &, const std::string &){return "lib";};
        trans[Part::Pre] = [](const std::string &k, const std::string &v) {return k;};
        trans[Part::Output] = [](const std::string &k, const std::string &v){return std::string{"/OUT:"}+k;};
        trans[Part::Input] = [](const std::string &k, const std::string &v){return k;};

        MSS_END();
    }

} } } }

#endif

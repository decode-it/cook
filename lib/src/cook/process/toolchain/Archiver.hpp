#ifndef HEADER_cook_process_toolchain_Archiver_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Archiver_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Manager.hpp"
#include "cook/OS.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace process { namespace toolchain { 

    inline bool configure_archiver(Manager & manager)
    {
        MSS_BEGIN(bool);

        manager.goc_element(Element::Archive, Language::Binary);
        
        MSS_END();
    }

} } } 

#endif

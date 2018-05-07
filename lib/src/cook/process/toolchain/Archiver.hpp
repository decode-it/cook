#ifndef HEADER_cook_process_toolchain_Archiver_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Archiver_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Interface.hpp"
#include "cook/OS.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace process { namespace toolchain { 

    class Archiver: public Interface
    {
    public:
        bool create(command::Archive::Ptr &ptr) const override
        {
            MSS_BEGIN(bool);
            ptr.reset(new command::Archive(key_values_map(), translator_map_ptr()));
            MSS_END();
        }
    };

} } } 

#endif

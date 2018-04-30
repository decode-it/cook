#ifndef HEADER_cook_process_toolchain_Archiver_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Archiver_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Interface.hpp"

namespace cook { namespace process { namespace toolchain { 

    class Archiver: public Interface
    {
    public:
        Archiver()
        {
        }

        bool set_brand(const std::string &brand) override
        {
            MSS_BEGIN(bool);
            MSS_END();
        }
    private:
    };

} } } 

#endif

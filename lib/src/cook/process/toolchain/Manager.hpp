#ifndef HEADER_cook_process_toolchain_Manager_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Manager_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Interface.hpp"
#include "cook/Language.hpp"
#include <map>

namespace cook { namespace process { namespace toolchain { 

    class Manager
    {
    public:
        Interface &compiler(Language);
        Interface &archiver();
        Interface &linker();

        bool set_brand(const std::string &brand);

    private:
        std::string brand_;
        std::map<Language, Interface::Ptr> compilers_;
        Interface::Ptr archiver_;
        Interface::Ptr linker_;
    };

} } } 

#endif

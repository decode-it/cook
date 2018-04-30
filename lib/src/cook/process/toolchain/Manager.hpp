#ifndef HEADER_cook_process_toolchain_Manager_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Manager_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Interface.hpp"
#include "cook/Language.hpp"
#include <map>

namespace cook { namespace process { namespace toolchain { 

    class Manager
    {
    public:
        Manager();

        const Interface &compiler(Language) const;
        const Interface &archiver() const {return archiver_();}
        const Interface &linker() const {return linker_();}

        bool set_brand(const std::string &brand);

    private:
        std::string brand_;
        mutable std::map<Language, Interface::Ptr> compilers_;
        mutable Interface::Ptr archiver_ptr_;
        mutable Interface::Ptr linker_ptr_;

        Interface &archiver_() const;
        Interface &linker_() const;
    };

} } } 

#endif

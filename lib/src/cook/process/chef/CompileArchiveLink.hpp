#ifndef HEADER_cook_process_chef_CompileArchiveLink_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_chef_CompileArchiveLink_hpp_ALREADY_INCLUDED

#include "cook/process/chef/Interface.hpp"
#include <set>

namespace cook { namespace process { namespace chef {

    struct CompileArchiveLink : public Interface
    {
        explicit CompileArchiveLink(bool execute_scripts);

        Result initialize() override;

        void set_compiler(Language language, SouschefPtr compiler);
        void clear_compiler(Language language);
        void set_linker(SouschefPtr linker);
        void set_archiver(SouschefPtr archiver);

    private:
        const bool execute_scripts_;

        std::list<SouschefPtr> generate_compile_only_steps_() const;

        std::map<Language, SouschefPtr> compilers_;
        SouschefPtr linker_;
        SouschefPtr archiver_;
    };

} } }

#endif

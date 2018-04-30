#ifndef HEADER_cook_process_chef_CompileArchiveLink_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_chef_CompileArchiveLink_hpp_ALREADY_INCLUDED

#include "cook/process/chef/Interface.hpp"
#include <set>

namespace cook { namespace process { namespace chef {

    struct CompileArchiveLink : public Interface
    {
        explicit CompileArchiveLink(const std::string &name);

        Result initialize() override;

        void set_compiler(Language language, SouschefPtr compiler);
        void clear_compiler(Language language);
        void set_linker(SouschefPtr linker);
        void set_archiver(SouschefPtr archiver);

    private:
        std::list<SouschefPtr> generate_compile_only_steps_() const;

        std::string name_;
        std::map<Language, SouschefPtr> compilers_;
        SouschefPtr linker_;
        SouschefPtr archiver_;
    };

} } }

#endif

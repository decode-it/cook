#ifndef HEADER_cook_process_chef_CompileLinkArchive_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_chef_CompileLinkArchive_hpp_ALREADY_INCLUDED

#include "cook/process/chef/Chef.hpp"
#include <set>

namespace cook { namespace process { namespace chef {

struct LinkArchiveChef : public Chef
{
    explicit LinkArchiveChef(const std::string &name);

    Result initialize() override;

    void set_compiler(Language language, AssistantPtr compiler);
    void clear_compiler(Language language);
    void set_linker(AssistantPtr linker);
    void set_archiver(AssistantPtr archiver);

private:
    std::list<AssistantPtr> generate_compile_only_steps_() const;

    std::string name_;
    std::map<Language, AssistantPtr> compilers_;
    AssistantPtr linker_;
    AssistantPtr archiver_;

};

} } }

#endif

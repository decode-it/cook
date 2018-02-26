#ifndef HEADER_cook_chef_CompileLinkArchive_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_CompileLinkArchive_hpp_ALREADY_INCLUDED

#include "cook/chef/Interface.hpp"
#include <set>

namespace cook { namespace toolchain {

class CompileLinkArchive : public Interface
{
public:
    explicit CompileLinkArchive(const std::string & name);

    std::string name() const override;
    Result initialize() override;
    Result generate_processors(model::Recipe * recipe, std::list<Step> & steps) override;

    void disable_build() override;

private:


    using StepList = std::list<Step>;

    StepList generate_compile_only_steps_() const;
    StepList generate_archive_steps_() const;
    StepList generate_link_steps_() const;

    std::string name_;

    std::map<Language, ChefPtr> compilers_;
    ChefPtr linker_;
    ChefPtr archiver_;

    StepList compile_only_steps_;
    StepList archive_steps_;
    StepList link_steps_;
};

} }

#endif

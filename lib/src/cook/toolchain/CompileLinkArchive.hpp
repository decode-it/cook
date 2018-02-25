#ifndef HEADER_cook_toolchain_CompileLinkArchive_hpp_ALREADY_INCLUDED
#define HEADER_cook_toolchain_CompileLinkArchive_hpp_ALREADY_INCLUDED

#include "cook/toolchain/Interface.hpp"

namespace cook { namespace toolchain {

class CompileLinkArchive : public Interface
{
public:
    explicit CompileLinkArchive(const std::string & name);

    std::string name() const override;

    Result generate_processors(model::Recipe * recipe, std::list<Step> & steps) override = 0;

private:
    Result generate_executable_processors_(std::list<Step> & steps) const;
    Result generate_shared_lib_processors_(std::list<Step> & steps) const;
    Result generate_static_lib_processors_(std::list<Step> & steps) const;
    Result generate_undefined_processors_(std::list<Step> & steps) const;

    std::string name_;
    ChefPtr compiler_;
    ChefPtr linker_;
    ChefPtr archiver_;
};

} }

#endif

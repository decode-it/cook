#ifndef HEADER_cook_generator_Naft_hpp_ALREADY_INCLUDED
#define HEADER_cook_generator_Naft_hpp_ALREADY_INCLUDED

#include "cook/generator/Interface.hpp"
#include "cook/model/Dirs.hpp"
#include "gubg/std/filesystem.hpp"

namespace cook { namespace generator {

class Naft : public Interface
{
public:
    std::string name() const override { return "naft"; }

    Result set_option(const std::string & option) override;

    bool can_process(const Context & context) const override;
    Result process(const Context & context) override;
};

} }

#endif

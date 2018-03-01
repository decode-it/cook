#ifndef HEADER_cook_staff_souschef_Archiver_hpp_ALREADY_INCLUDED
#define HEADER_cook_staff_souschef_Archiver_hpp_ALREADY_INCLUDED

#include "cook/staff/souschef/Interface.hpp"
#include <set>

namespace cook { namespace staff { namespace souschef {

class Archiver : public Interface
{
public:
    std::string description() const override { return "Archiver"; }
    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    virtual ingredient::File construct_archive_file(const Context &context) const;
    virtual build::CommandPtr archive_command(const Context & context) const;
};

} } }



#endif

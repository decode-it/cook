#ifndef HEADER_cook_process_souschef_IncludePathSetter_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_souschef_IncludePathSetter_hpp_ALREADY_INCLUDED

#include "cook/process/souschef/Interface.hpp"
#include "gubg/stream.hpp"

namespace cook { namespace process { namespace souschef {

class IncludePathSetter : public Interface
{
public:
    IncludePathSetter(Language language);

    std::string description() const override { return gubg::stream([&](auto & os) { os << language_ << " include path setter"; }); }
    Result process(const Context &, model::Recipe &) const override;

private:
    Language language_;
};

} } }

#endif

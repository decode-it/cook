#ifndef HEADER_cook_chef_assistant_IncludePathSetter_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_assistant_IncludePathSetter_hpp_ALREADY_INCLUDED

#include "cook/chef/Assistant.hpp"
#include "gubg/stream.hpp"

namespace cook { namespace chef { namespace assistant {

class IncludePathSetter : public Assistant
{
public:
    IncludePathSetter(Language language);

    std::string description() const override { return gubg::stream([&](auto & os) { os << language_ << " include path setter"; }); }
    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    Language language_;
};

} } }

#endif

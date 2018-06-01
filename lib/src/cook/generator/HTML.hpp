#ifndef HEADER_cook_generator_HTML_hpp_ALREADY_INCLUDED
#define HEADER_cook_generator_HTML_hpp_ALREADY_INCLUDED

#include "cook/generator/Interface.hpp"

namespace cook { namespace generator {

class HTML : public Interface
{
public:
    std::string name() const override { return "html"; }

    Result set_option(const std::string & option) override;

    bool can_process(const Context & context) const override;
    Result process(const Context & context) override;

private:
    std::string ns_ = "html";
};

} }

#endif

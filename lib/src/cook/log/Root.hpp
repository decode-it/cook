#ifndef HEADER_cook_log_Root_hpp_ALREADY_INCLUDED
#define HEADER_cook_log_Root_hpp_ALREADY_INCLUDED

#include "cook/log/Scope.hpp"
#include "gubg/naft/Document.hpp"

namespace cook { namespace log {

struct Root : private gubg::naft::Document, public Scope
{
    explicit Root(std::ostream & oss = std::cout);

private:
    Root(const Root &) = delete;
    Root(Root &&) = delete;
    Root & operator=(const Root &) = delete;
    Root & operator=(Root &&) = delete;
};

} }

#endif

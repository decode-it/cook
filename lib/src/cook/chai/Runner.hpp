#ifndef HEADER_cook_chai_Runner_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Runner_hpp_ALREADY_INCLUDED

#include "cook/model/Book.hpp"
#include <string>
#include <stack>

namespace cook { namespace chai {

struct Runner
{
    Runner();
    ~Runner();

    void load(const std::list<std::string> & recipes);
    void report_error(const std::string & message);

    const model::Book & root() const { return root_; }

private:
    void include_(const std::string & file);

    struct D;
    model::Book root_;
    std::unique_ptr<D> d;
};

} }

#endif


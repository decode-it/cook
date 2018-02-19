#ifndef HEADER_cook_chai_Runner_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Runner_hpp_ALREADY_INCLUDED

#include "cook/model/Book.hpp"
#include "gubg/std/filesystem.hpp"
#include <string>
#include <stack>

namespace cook { namespace chai {

struct Runner
{
    Runner();
    ~Runner();

    bool load(const std::list<std::string> & recipes);

    const model::Book & root() const { return root_; }

private:
    void include_(const std::string & file);
    void reset_engine_();
    std::filesystem::path generate_file_path_(const std::string & file) const;

    struct D;
    model::Book root_;
    std::unique_ptr<D> d;
};

} }

#endif


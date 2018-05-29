#include "cook/model/Dirs.hpp"

namespace cook { namespace model {

Dirs::Dirs()
{
    set_output("build");
    set_temporary(".cook");
}

void Dirs::set_output(const std::filesystem::path & dir)
{
    output_ = dir;
}

void Dirs::set_temporary(const std::filesystem::path & dir)
{
    temporary_ = dir;
}

std::filesystem::path Dirs::output() const
{
    return output_;
}

std::filesystem::path Dirs::temporary() const
{
    return gubg::filesystem::combine(output(), temporary_);
}

} }

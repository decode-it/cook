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
        
void Dirs::add_include_dir(const std::filesystem::path & dir)
{
    include_dirs_.push_back(std::filesystem::absolute(dir));
}

std::filesystem::path Dirs::output(bool make_absolute) const
{
    if (make_absolute)
        return gubg::filesystem::combine(std::filesystem::current_path(), output_);
    else
        return output_;
}

std::filesystem::path Dirs::temporary(bool make_absolute) const
{
    return gubg::filesystem::combine(output(make_absolute), temporary_);
}
        
gubg::Range<Dirs::IncludeDirIt> Dirs::include_dirs() const
{
    return gubg::make_range(include_dirs_);
}

} }

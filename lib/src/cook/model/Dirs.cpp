#include "cook/model/Dirs.hpp"

namespace cook { namespace model {

void Dirs::set_recipe(const std::filesystem::path & dir)
{
    recipe_ = dir;
}

void Dirs::set_output(const std::filesystem::path & dir)
{
    output_ = dir;
}

void Dirs::set_temporary(const std::filesystem::path & dir)
{
    temporary_ = dir;
}

std::filesystem::path Dirs::recipe() const
{
    return recipe_;
}

std::filesystem::path Dirs::output() const
{
    if (output_.is_absolute())
        return output_;
    else
        return recipe()/output_;
}

std::filesystem::path Dirs::temporary() const
{
    if (temporary().is_absolute())
        return temporary_;
    else
        return output() / temporary_;
}

} }

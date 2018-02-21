#include "cook/model/Environment.hpp"

namespace cook { namespace model {

void Environment::Dirs::set_recipe(const std::filesystem::path & dir)
{
    recipe_ = dir;
}
void Environment::Dirs::set_output(const std::filesystem::path & dir) { output_ = dir; }
void Environment::Dirs::set_temporary(const std::filesystem::path & dir) { temporary_ = dir; }

std::filesystem::path Environment::Dirs::recipe() const
{
    return recipe_; }

std::filesystem::path Environment::Dirs::output() const
{
    return output_.is_absolute()    ? output_    : recipe() / output_;
}
std::filesystem::path Environment::Dirs::temporary() const
{
    return temporary_.is_absolute() ? temporary_ : output() / temporary_;
}


bool Environment::Dirs::operator==(const Dirs & rhs) const
{
    return recipe_ == rhs.recipe_ && output_ == rhs.output_ && temporary_ == rhs.temporary_;
}


bool Environment::operator==(const Environment & rhs) const
{
    return dirs == rhs.dirs && toolchain == rhs.toolchain && is_equal_(&rhs);
}

}

               }


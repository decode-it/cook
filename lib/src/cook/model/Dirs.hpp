#ifndef HEADER_cook_model_Dirs_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Dirs_hpp_ALREADY_INCLUDED

#include "gubg/std/filesystem.hpp"

namespace cook { namespace model {

struct Dirs
{
    void set_recipe(const std::filesystem::path & dir);
    void set_output(const std::filesystem::path & dir);
    void set_temporary(const std::filesystem::path & dir);

    std::filesystem::path recipe() const;
    std::filesystem::path output() const;
    std::filesystem::path temporary() const;
private:
    std::filesystem::path recipe_;
    std::filesystem::path output_;
    std::filesystem::path temporary_;
};

} }

#endif

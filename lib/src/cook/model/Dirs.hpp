#ifndef HEADER_cook_model_Dirs_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Dirs_hpp_ALREADY_INCLUDED

#include "gubg/std/filesystem.hpp"

namespace cook { namespace model {

    struct Dirs
    {
        Dirs();

        void set_output(const std::filesystem::path & dir);
        void set_temporary(const std::filesystem::path & dir);

        std::filesystem::path output(bool make_absolute = false) const;
        std::filesystem::path temporary(bool make_absolute = false) const;

        private:
        std::filesystem::path output_;
        std::filesystem::path temporary_;
    };

} }

#endif

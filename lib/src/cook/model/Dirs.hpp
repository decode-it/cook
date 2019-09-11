#ifndef HEADER_cook_model_Dirs_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Dirs_hpp_ALREADY_INCLUDED

#include "gubg/std/filesystem.hpp"
#include "gubg/Range.hpp"
#include <vector>

namespace cook { namespace model {

    struct Dirs
    {
        using IncludeDirVct = std::vector<std::filesystem::path>;
        using IncludeDirIt = IncludeDirVct::const_iterator;

        Dirs();

        void set_output(const std::filesystem::path & dir);
        void set_temporary(const std::filesystem::path & dir);
        void add_include_dir(const std::filesystem::path & dir);

        std::filesystem::path output(bool make_absolute = false) const;
        std::filesystem::path temporary(bool make_absolute = false) const;
        gubg::Range<IncludeDirIt> include_dirs() const;

        private:
        std::filesystem::path output_;
        std::filesystem::path temporary_;
        IncludeDirVct include_dirs_;
    };

} }

#endif

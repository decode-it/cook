#ifndef HEADER_cook_model_Env_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Env_hpp_ALREADY_INCLUDED

#include "gubg/std/filesystem.hpp"

namespace cook { namespace model { 

    class Env
    {
    public:
        void set_build_dir(const std::string &build_dir) {build_dir_ = build_dir;}
        const std::filesystem::path build_dir() const {return build_dir_;}
    private:
        std::filesystem::path build_dir_;
    };

} } 

#endif

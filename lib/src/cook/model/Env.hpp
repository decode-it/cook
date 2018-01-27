#ifndef HEADER_cook_model_Env_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Env_hpp_ALREADY_INCLUDED

#include "gubg/std/filesystem.hpp"
#include <string>
#include <map>

namespace cook { namespace model { 

    class Env
    {
    public:
        Env()
            : project_dir_(""),
              act_output_dir_("output"),
              act_temp_dir_("build")
        {
            reset_dirs_();
        }

        void set_project_dir(const std::string & dir)  { project_dir_ = dir; reset_dirs_(); }
        void set_temp_dir(const std::string & dir)     { act_temp_dir_ = dir; reset_dirs_(); }
        void set_output_dir(const std::string & dir)   { act_output_dir_ = dir; reset_dirs_(); }

        const std::filesystem::path & output_dir() const    { return output_dir_; }
        const std::filesystem::path project_dir() const     { return project_dir_; }
        const std::filesystem::path temp_dir() const        { return temp_dir_; }

    private:
        void reset_dirs_()
        {
            output_dir_ = gubg::filesystem::combine(project_dir_, act_output_dir_);
            temp_dir_ = gubg::filesystem::combine(project_dir_, act_temp_dir_);
        }

        std::filesystem::path project_dir_;
        std::filesystem::path output_dir_;
        std::filesystem::path temp_dir_;
        std::filesystem::path act_temp_dir_;
        std::filesystem::path act_output_dir_;
    };

} } 

#endif

#ifndef HEADER_cook_model_Environment_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Environment_hpp_ALREADY_INCLUDED

#include "gubg/std/filesystem.hpp"
#include <string>
#include <list>

namespace cook { namespace model {

class Environment
{
public:
    using Variable = std::pair<std::string, std::string>;

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


    virtual bool set_variables(const std::list<Variable> & variables) = 0;

    Dirs dirs;
    std::string toolchain;
};

} }

#endif

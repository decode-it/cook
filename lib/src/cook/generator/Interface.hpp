#ifndef HEADER_cook_generator_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_generator_Interface_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "cook/model/Dirs.hpp"
#include "gubg/std/filesystem.hpp"

namespace cook {

class Context;

}

namespace cook { namespace generator {

class Interface
{
public:
    virtual std::string name() const = 0;

    virtual Result set_option(const std::string & option) = 0;

    virtual bool can_process(const Context & context) const = 0;
    virtual Result process(std::ostream & ofs, const Context & context) = 0;

    virtual std::filesystem::path output_filename(const model::Dirs & dirs) const
    {
        const std::string default_name = default_filename();

        if (false) {}
        else if (filename_.empty())
            return dirs.output() / default_name;
        else if (filename_.back() == std::filesystem::path::preferred_separator)
            return std::filesystem::path(filename_) / default_name;
        else
            return std::filesystem::path(filename_);
    }
protected:
    void set_filename(const std::string & filename)
    {
        filename_ = filename;
    }

private:
    virtual std::string default_filename() const
    {
        return "recipes." + name();
    }

    std::string filename_;
};

} }

#endif

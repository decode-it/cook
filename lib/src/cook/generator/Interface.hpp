#ifndef HEADER_cook_generator_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_generator_Interface_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "cook/Context.hpp"
#include "cook/util/File.hpp"
#include "gubg/std/filesystem.hpp"
#include <fstream>

namespace cook { namespace generator {

class Interface
{
public:
    virtual ~Interface() {}

    virtual std::string name() const = 0;

    virtual Result set_option(const std::string & option) = 0;

    virtual bool can_process(const Context & context) const = 0;
    virtual Result process(const Context & context) = 0;

    virtual std::filesystem::path output_filename(const model::Dirs & dirs) const
    {
        const std::string default_name = default_filename();

        if (filename_.empty())
            return dirs.output() / default_name;
        if (filename_.back() == std::filesystem::path::preferred_separator)
            return std::filesystem::path(filename_) / default_name;
        return std::filesystem::path(filename_);
    }

protected:
    Result open_output_stream(const Context & context, std::ofstream & ofs)
    {
        MSS_BEGIN(Result);
        MSS(!ofs.is_open());

        std::filesystem::path path = output_filename(context.dirs());
        MSS(util::open_file(path, ofs));

        MSS_END();
    }


    void set_filename(const std::string & filename)
    {
        filename_ = filename;
    }

    const std::string & get_filename() const
    {
        return filename_;
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

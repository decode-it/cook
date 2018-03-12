#include "cook/generator/Ninja.hpp"
#include "cook/Context.hpp"

namespace cook { namespace generator { 

    Result Ninja::set_option(const std::string & option)
    {
        MSS_BEGIN(Result);
        set_filename(option);
        MSS_END();
    }

    bool Ninja::can_process(const Context & context) const
    {
        return context.menu().is_valid();
    }

    Result Ninja::process(std::ostream & ofs, const Context & context)
    {
        MSS_BEGIN(Result);
        MSS_END();
    }

    std::filesystem::path Ninja::output_filename(const model::Dirs & dirs) const
    {
        return dirs.output() / "build.ninja";
    }

} } 

#include "cook/generator/Naft.hpp"
#include "cook/util/File.hpp"
#include "cook/algo/TopologicalOrder.hpp"
#include "cook/Context.hpp"


namespace cook { namespace generator {

Result Naft::set_option(const std::string & option)
{
    filename_ = option;
}

bool Naft::can_process(const Context & context) const
{
    return context.menu().is_valid();
}

Result Naft::process(const Context & context)
{

}

std::filesystem::path Naft::output_filename_(const model::Dirs & dirs) const
{
    const static std::string default_extension = "naft";
    const static std::string default_name = "recipes." + default_extension;

    if (false) {}
    else if (filename_.empty())
        return dirs.output() / default_name;
    else if (filename_.back() == std::filesystem::path::preferred_separator)
        return std::filesystem::path(filename_) / default_name;
    else
        return std::filesystem::path(filename_);
}

} }

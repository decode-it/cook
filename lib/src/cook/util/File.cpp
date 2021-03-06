#include "cook/util/File.hpp"

namespace cook { namespace util {

Result open_file(const std::filesystem::path & path, std::ofstream & ofs)
{
    MSS_BEGIN(Result);
    auto s = log::scope("open file", [&](auto & n) { n.attr("path", path); });

    std::filesystem::path parent = path.parent_path();
    if (!parent.empty() && !std::filesystem::is_directory(parent))
        MSG_MSS(std::filesystem::create_directories(parent), Error, "Unable to create directory '" << parent.string() << "'");

    ofs.open(path.string());
    MSG_MSS(ofs.good(), Error, "Unable to create file '" << path.string() << "'");

    MSS_END();
}

std::filesystem::path get_from_to_path(const model::Recipe & from, const model::Recipe & to)
{
    return get_from_to_path(from.working_directory(), to.working_directory());
}
std::filesystem::path get_from_to_path(const model::Recipe & from, const std::filesystem::path & to)
{
    return get_from_to_path(from.working_directory(), to);
}
std::filesystem::path get_from_to_path(const std::filesystem::path & from, const model::Recipe & to)
{
    return get_from_to_path(from, to.working_directory());
}
std::filesystem::path get_from_to_path(const std::filesystem::path & from, const std::filesystem::path & to)
{
    std::filesystem::path path;
    const auto from_comb = gubg::filesystem::combine(std::filesystem::current_path(), from);
    const auto to_comb = gubg::filesystem::combine(std::filesystem::current_path(), to);
    gubg::filesystem::get_from_to(path, from_comb, to_comb);
    return path;
}

} }


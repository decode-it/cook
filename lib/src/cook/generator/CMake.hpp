#ifndef HEADER_cook_generator_CMake_hpp_ALREADY_INCLUDED
#define HEADER_cook_generator_CMake_hpp_ALREADY_INCLUDED

#include "cook/generator/Interface.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/model/Dirs.hpp"

namespace cook { namespace generator {

class CMake : public Interface
{
public:
    std::string name() const override { return "CMake"; }

    Result set_option(const std::string & option) override;

    bool can_process(const Context & context) const override;
    Result process(const Context & context) override;

private:
    void add_library_(std::ofstream & ofs, const model::Recipe & recipe) const;
    void add_executable_(std::ofstream & ofs, const model::Recipe & recipe) const;
    void add_interface_(std::ofstream & ofs, const model::Recipe & recipe) const;

    std::string recipe_name_(const model::Recipe & recipe) const;
    std::string deduce_library_type_(const model::Recipe & recipe) const;

    void add_source_and_header_(std::ofstream & ofs, const model::Recipe & recipe, bool add_headers) const;
    void set_target_properties_(std::ofstream & ofs, const model::Recipe & recipe, const std::string &keyword) const;
    void set_link_paths_(std::ofstream & ofs, const model::Recipe & recipe) const;

    std::filesystem::path output_filename_(const model::Dirs & dirs) const;

    bool contains_sources_(const model::Recipe & recipe) const;

private:
    std::string filename_;
};

} }

#endif

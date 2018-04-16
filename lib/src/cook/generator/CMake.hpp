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
    Result process(std::ostream & ofs, const Context & context) override;

private:
    std::string default_filename() const override;

    void add_library_(std::ostream &ofs, const model::Recipe & recipe, const std::filesystem::path & output_to_source) const;
    void add_executable_(std::ostream &ofs, const model::Recipe & recipe, const std::filesystem::path &output_to_source) const;
    void add_interface_(std::ostream & ofs, const model::Recipe & recipe, const std::filesystem::path & output_to_source) const;

    std::string recipe_name_(const model::Recipe & recipe) const;
    std::string deduce_library_type_(const model::Recipe & recipe) const;

    void add_source_and_header_(std::ostream & ofs, const model::Recipe & recipe, bool add_headers, const std::filesystem::path &output_to_source) const;
    void set_target_properties_(std::ostream & ofs, const model::Recipe & recipe, const std::string &keyword, const std::filesystem::path & output_to_source) const;
    void set_link_paths_(std::ostream & oss, const model::Recipe & recipe, const std::filesystem::path & output_to_source) const;

    bool contains_sources_(const model::Recipe & recipe) const;
};

} }

#endif

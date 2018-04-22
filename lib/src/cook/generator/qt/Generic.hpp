#ifndef HEADER_cook_generator_qt_Generic_hpp_ALREADY_INCLUDED
#define HEADER_cook_generator_qt_Generic_hpp_ALREADY_INCLUDED

#include "cook/generator/Interface.hpp"

namespace cook { namespace generator { namespace qt {

class Generic : public Interface
{
public:
    std::string name() const override { return "qt.generic"; }

    Result set_option(const std::string & option) override;

    bool can_process(const Context & context) const override;
    Result process(const Context & context) override;

private:
    Result open_output_stream(const std::string &project_name, const std::string &postfix, std::ofstream & ofs) const;
    Result write_files_(std::ofstream & ofs, const model::Recipe & recipe, const std::filesystem::path & to_recipe) const;
    Result write_includes_(std::ofstream & ofs, const model::Recipe & recipe, const std::filesystem::path & to_recipe) const;
    Result write_config_(std::ofstream & ofs, const model::Recipe & recipe) const;



private:
    std::filesystem::path output_dir_;
};

} } }

#endif

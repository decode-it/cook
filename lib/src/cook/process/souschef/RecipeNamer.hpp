#ifndef HEADER_cook_process_souschef_RecipeNamer_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_souschef_RecipeNamer_hpp_ALREADY_INCLUDED

#include "cook/process/souschef/Interface.hpp"
#include "gubg/stream.hpp"

namespace cook { namespace process { namespace souschef {

class RecipeNamer : public Interface
{
public:
    std::string description() const override { return "Recipe name"; }
    Result process(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context & context) const override;

private:
    void update_build_target_name(model::Recipe & recipe) const;
    std::filesystem::path dynamic_library_filename(const std::string &build_target_name) const;
    std::filesystem::path executable_filename(const std::string &build_target_name) const;
    std::filesystem::path static_library_filename(const std::string &build_target_name) const;
};

} } }



#endif

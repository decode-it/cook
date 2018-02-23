#ifndef HEADER_cook_visualizer_Graphviz_hpp_ALREADY_INCLUDED
#define HEADER_cook_visualizer_Graphviz_hpp_ALREADY_INCLUDED

#include "cook/visualizer/Interface.hpp"

namespace cook { namespace visualizer {

class Graphviz : public Interface
{
public:
    enum class Color
    {
        Root = 0,
        InTree = 1,
        OutsideTree = 2,
        Undefined = 3
    };
    using ColorPropertyMap = std::map<std::string, std::string>;

    Graphviz();

    std::string name() const override;

    std::shared_ptr<Interface> clone() const override;
    Result set_option(const std::string & option) override;

    bool can_process(const model::DependencyGraph & graph) const override;
    Result process(const model::DependencyGraph & graph, const model::Environment & environment) override;

    const ColorPropertyMap & color_property_map(Color c) const;
    ColorPropertyMap & color_property_map(Color c);

private:
    void process_(std::ostream & oss, const model::DependencyGraph & graph) const;

    void write_node_desc_(std::ostream & oss, const std::string & id, const std::string & uri, Color color) const;
    void write_node_(std::ostream & oss, model::Recipe * recipe, Color color) const;
    void write_edges_(std::ostream & oss, model::Recipe * recipe, model::Book * root) const;
    void write_header_(std::ostream & oss) const;
    void write_footer_(std::ostream & oss) const;

    std::string color_property_string(Color color) const;

    std::filesystem::path output_filename(const model::Environment & environment) const;

    std::string filename;
    std::array<ColorPropertyMap, 4> color_map_;
};

} }

#endif

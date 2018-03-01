#ifndef HEADER_cook_visualizer_Graphviz_hpp_ALREADY_INCLUDED
#define HEADER_cook_visualizer_Graphviz_hpp_ALREADY_INCLUDED

#include "cook/visualizer/Interface.hpp"
#include "cook/process/Menu.hpp"
#include "cook/Context.hpp"

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

    Result set_option(const std::string & option) override;

    bool can_process(const Context & context) const override;
    Result process(const Context & context) override;

    const ColorPropertyMap & color_property_map(Color src, Color dst) const;
    const ColorPropertyMap & color_property_map(Color c) const;
    ColorPropertyMap & color_property_map(Color src, Color dst);
    ColorPropertyMap & color_property_map(Color c);

private:
    void process_(std::ostream & oss, const Context &context) const;
    void write_node_desc_(std::ostream & oss, const std::string & id, const std::string & uri, Color color) const;
    void write_header_(std::ostream & oss) const;
    void write_footer_(std::ostream & oss) const;

    std::string color_property_string_(const ColorPropertyMap & map) const;

    std::filesystem::path output_filename(const model::Dirs & dirs) const;

    std::string filename;
    std::map<Color, ColorPropertyMap> node_color_map_;
    std::map<std::pair<Color, Color>, ColorPropertyMap> edge_color_map_;
};

} }

#endif

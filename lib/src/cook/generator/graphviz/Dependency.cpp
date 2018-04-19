#include "cook/generator/graphviz/Dependency.hpp"
#include "cook/model/Book.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/algo/Visit.hpp"
#include "cook/algo/TopologicalOrder.hpp"
#include "cook/util/File.hpp"
#include <unordered_map>
#include <algorithm>

using namespace cook::model;

namespace cook { namespace generator { namespace graphviz {

namespace  {

static const char special_separator = '_';

std::string node_name(Recipe * recipe)
{
    auto str = recipe->uri().string('_');
    std::replace(RANGE(str), '-', '_');
    return str;
}

std::string unexisting_node_name(Recipe * recipe, const Uri & uri)
{
    std::ostringstream oss;
    oss << special_separator << recipe->uri().string(special_separator) << special_separator << special_separator << uri.string(special_separator);
    return oss.str();

}

using NodeMap = std::unordered_map<Recipe *, Dependency::Color>;

}

Result Dependency::process(const Context  & context)
{
    MSS_BEGIN(Result);

    std::ofstream ofs;
    MSS(open_output_stream(context, ofs));

    NodeMap nodes;

    const auto & menu = context.menu();


    // add all the root nodes
    for(Recipe * recipe : menu.root_recipes())
        nodes.emplace(recipe, Dependency::Color::Root);

    // add all the tree nodes
    {
        auto initializer = [&](auto & stack)
        {
            for (model::Recipe * recipe : menu.root_recipes())
                stack.push(recipe);
        };

        auto add_tree_nodes = [&](model::Recipe * cur, auto & stack)
        {
            // add to the map (if not present)
            nodes.emplace(cur, Dependency::Color::InTree);

            for(model::Recipe * dep : cur->dependencies())
                if (dep)
                    stack.push(dep);
            return true;
        };

        algo::visit(initializer, add_tree_nodes);
    };

    // add all the recipes in the root book
    for(model::Recipe * recipe : context.lib().list_all_recipes())
        nodes.emplace(recipe, Dependency::Color::OutsideTree);

    // and now write out all the information
    write_header_(ofs);

    // write out the nodes
    for(const auto & p1 : nodes)
    {
        Recipe * recipe = p1.first;
        Color color = p1.second;
        write_node_desc_(ofs, node_name(recipe), recipe->uri().string(), color);
    }

    for(const auto & p1 : nodes)
    {
        Color src_color = p1.second;
        Recipe * src = p1.first;
        const std::string & src_id = node_name(src);

        for(const auto & p : src->dependency_pairs())
        {
            // make sure we try to resolve it
            Recipe * tgt = p.second;
            Color dst_color;
            std::string dst_id;

            if (tgt == nullptr)
            {
                dst_id = unexisting_node_name(src, p.first);
                dst_color = Color::Undefined;

                write_node_desc_(ofs, dst_id, p.first.string(), dst_color);
            }
            else
            {
                dst_id = node_name(tgt);
                dst_color = nodes[tgt];
            }

            const std::string options = color_property_string_(color_property_map(src_color, dst_color));
            ofs << src_id << " -> " << dst_id << " [" << options << "];" << std::endl;
        }
    }

    write_footer_(ofs);

    MSS_END();
}

namespace {

Dependency::ColorPropertyMap defaultNodeColorPropertyMap(Dependency::Color c)
{
    Dependency::ColorPropertyMap res;

    switch(c)
    {
        case Dependency::Color::Root:
            res["color"] = "green";
            res["fontcolor"] = "black";
            break;

        case Dependency::Color::InTree:
            break;

        case Dependency::Color::OutsideTree:
            res["color"] = "grey";
            res["fontcolor"] = "grey";
            break;

        default:
            res["color"] = "red";
            res["fontcolor"] = "red";
            break;
    }

    return res;
}

Dependency::ColorPropertyMap defaultEdgeColorPropertyMap(Dependency::Color src, Dependency::Color dst)
{
    Dependency::Color c = std::max(src, dst);

    Dependency::ColorPropertyMap res;
    switch(c)
    {
        case Dependency::Color::Root:
            break;

        case Dependency::Color::InTree:
            break;

        case Dependency::Color::OutsideTree:
            res["color"] = "grey";
            break;

        default:
            res["color"] = "red";
            break;
    }

    return res;
}

}

Dependency::Dependency()
{
    const static auto all_colors = {Color::Root, Color::InTree, Color::OutsideTree, Color::Undefined};

    for (Color c : all_colors)
    {
        node_color_map_[c] = defaultNodeColorPropertyMap(c);

        for (Color c2 : all_colors)
            edge_color_map_[std::make_pair(c, c2)] = defaultEdgeColorPropertyMap(c, c2);
    }
}



const Dependency::ColorPropertyMap & Dependency::color_property_map(Color c) const
{
    return node_color_map_.find(c)->second;
}

const Dependency::ColorPropertyMap & Dependency::color_property_map(Color src, Color dst) const
{
    return edge_color_map_.find(std::make_pair(src, dst))->second;
}

Dependency::ColorPropertyMap & Dependency::color_property_map(Color c)
{
    return node_color_map_[c];
}

Dependency::ColorPropertyMap & Dependency::color_property_map(Color src, Color dst)
{
    return edge_color_map_[std::make_pair(src, dst)];
}

std::string Dependency::color_property_string_(const ColorPropertyMap & map) const
{
    std::ostringstream oss;
    for(auto it = map.begin(); it != map.end(); ++it)
    {
        if (it != map.begin())
            oss << ", ";
        oss << it->first << "=\"" << it->second << "\"";
    }

    return oss.str();
}

void Dependency::write_node_desc_(std::ostream & oss, const std::string & id, const std::string & uri, Color color) const
{
    std::string cps = color_property_string_(color_property_map(color));

    oss << id << " [label=\"" << uri << "\", " << cps << "];" << std::endl;
}

void Dependency::write_header_(std::ostream & oss) const
{
    oss << "digraph G {" << std::endl;
}

void Dependency::write_footer_(std::ostream & oss) const
{
    oss << "}" << std::endl;
}

std::string Dependency::default_filename() const
{
    return "recipes.graphviz";
}

std::string Dependency::name() const
{
    return "graphviz.dependency";
}

Result Dependency::set_option(const std::string & option)
{
    MSS_BEGIN(Result);

    filename = option;

    MSS_END();

}

bool Dependency::can_process(const Context &context) const
{
    return true;
}

} } }

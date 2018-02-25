#include "cook/visualizer/Graphviz.hpp"
#include "cook/model/Book.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/algo/Visit.hpp"
#include "cook/algo/Recipe.hpp"
#include <unordered_map>

using namespace cook::model;

namespace cook { namespace visualizer {

namespace  {

static const char special_separator = '_';

std::string node_name(Recipe * recipe)
{
    return recipe->uri().string('_');
}

std::string unexisting_node_name(Recipe * recipe, const Uri & uri)
{
    std::ostringstream oss;
    oss << special_separator << recipe->uri().string(special_separator) << special_separator << special_separator << uri.string(special_separator);
    return oss.str();

}

using NodeMap = std::unordered_map<Recipe *, Graphviz::Color>;

}

void Graphviz::process_(std::ostream & oss, const Menu & graph) const
{
    NodeMap nodes;

    // add all the root nodes
    for(Recipe * recipe : graph.root_recipes())
        nodes.emplace(recipe, Graphviz::Color::Root);

    // add all the tree nodes
    {
        auto initializer = [&](auto & stack)
        {
            for (model::Recipe * recipe : graph.root_recipes())
                stack.push(recipe);
        };

        auto add_tree_nodes = [&](model::Recipe * cur, auto & stack)
        {
            // add to the map (if not present)
            nodes.emplace(cur, Graphviz::Color::InTree);

            for(model::Recipe * dep : cur->dependencies())
                if (dep)
                    stack.push(dep);
            return true;
        };

        algo::visit(initializer, add_tree_nodes);
    };

    // add all the recipes in the root book
    std::stack<Book*> todo;
    todo.push(graph.root_book());

    while(!todo.empty())
    {
        Book * cur = todo.top();
        todo.pop();

        for(auto b : cur->books())
            todo.push(b);

        for(auto c : cur->recipes())
            nodes.emplace(c, Graphviz::Color::OutsideTree);
    }

    // and now write out all the information
    write_header_(oss);

    // write out the nodes
    for(const auto & p1 : nodes)
    {
        Recipe * recipe = p1.first;
        Color color = p1.second;
        write_node_desc_(oss, node_name(recipe), recipe->uri().string(), color);
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
            if (!tgt)
                algo::resolve_dependency(tgt, p.first, src->parent(), graph.root_book());

            Color dst_color;
            std::string dst_id;

            if (!tgt)
            {
                write_node_desc_(oss, dst_id, p.first.string(), dst_color);

                dst_id = unexisting_node_name(src, p.first);
                dst_color = Color::Undefined;
            }
            else
            {
                dst_id = node_name(tgt);
                dst_color = nodes[tgt];
            }

            const std::string options = color_property_string_(color_property_map(src_color, dst_color));
            oss << src_id << " -> " << dst_id << " [" << options << "];" << std::endl;
        }
    }

    write_footer_(oss);
}

namespace {

Graphviz::ColorPropertyMap defaultNodeColorPropertyMap(Graphviz::Color c)
{
    Graphviz::ColorPropertyMap res;

    switch(c)
    {
        case Graphviz::Color::Root:
            res["color"] = "green";
            res["fontcolor"] = "black";
            break;

        case Graphviz::Color::InTree:
            break;

        case Graphviz::Color::OutsideTree:
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

Graphviz::ColorPropertyMap defaultEdgeColorPropertyMap(Graphviz::Color src, Graphviz::Color dst)
{
    Graphviz::Color c = std::max(src, dst);

    Graphviz::ColorPropertyMap res;
    switch(c)
    {
        case Graphviz::Color::Root:
            break;

        case Graphviz::Color::InTree:
            break;

        case Graphviz::Color::OutsideTree:
            res["color"] = "grey";
            break;

        default:
            res["color"] = "red";
            break;
    }

    return res;
}

}

Graphviz::Graphviz()
{
    const static auto all_colors = {Color::Root, Color::InTree, Color::OutsideTree, Color::Undefined};

    for (Color c : all_colors)
    {
        node_color_map_[c] = defaultNodeColorPropertyMap(c);

        for (Color c2 : all_colors)
            edge_color_map_[std::make_pair(c, c2)] = defaultEdgeColorPropertyMap(c, c2);
    }
}



const Graphviz::ColorPropertyMap & Graphviz::color_property_map(Color c) const
{
    return node_color_map_.find(c)->second;
}

const Graphviz::ColorPropertyMap & Graphviz::color_property_map(Color src, Color dst) const
{
    return edge_color_map_.find(std::make_pair(src, dst))->second;
}

Graphviz::ColorPropertyMap & Graphviz::color_property_map(Color c)
{
    return node_color_map_[c];
}

Graphviz::ColorPropertyMap & Graphviz::color_property_map(Color src, Color dst)
{
    return edge_color_map_[std::make_pair(src, dst)];
}

std::string Graphviz::color_property_string_(const ColorPropertyMap & map) const
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

void Graphviz::write_node_desc_(std::ostream & oss, const std::string & id, const std::string & uri, Color color) const
{
    std::string cps = color_property_string_(color_property_map(color));

    oss << id << " [label=\"" << uri << "\", " << cps << "];" << std::endl;
}

void Graphviz::write_header_(std::ostream & oss) const
{
    oss << "digraph G {" << std::endl;
}

void Graphviz::write_footer_(std::ostream & oss) const
{
    oss << "}" << std::endl;
}

std::filesystem::path Graphviz::output_filename(const model::Environment & environment) const
{
    const static std::string default_extension = "graphviz";
    const static std::string default_name = "recipes." + default_extension;

    if (false) {}
    else if (filename.empty())
        return environment.dirs.output() / default_name;
    else if (filename.back() == std::filesystem::path::preferred_separator)
        return std::filesystem::path(filename) / default_name;
    else
        return std::filesystem::path(filename);
}

std::string Graphviz::name() const
{
    return "graphviz";
}

Result Graphviz::set_option(const std::string & option)
{
    MSS_BEGIN(Result);

    filename = option;

    MSS_END();

}

bool Graphviz::can_process(const model::Menu & graph) const
{
    return true;
}

Result Graphviz::process(const model::Menu & graph, const model::Environment & environment)
{
    MSS_BEGIN(Result);

    // create the output stream
    std::ofstream ofs;
    {
        std::filesystem::path p = output_filename(environment);

        std::filesystem::path parent = p.parent_path();
        if (!parent.empty())
            MSG_MSS(std::filesystem::create_directories(parent), Error, "Unable to create directory '" << p.parent_path() << "'");

        ofs.open(p.string());
        MSG_MSS(ofs.good(), Error, "Unable to create file '" << p.string() << "'");
    }

    process_(ofs, graph);

    MSS_END();
}

} }

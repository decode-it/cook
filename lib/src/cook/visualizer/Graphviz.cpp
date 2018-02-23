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



std::string get_color(Graphviz::Color c)
{
    switch(c)
    {
        case Graphviz::Color::Root: return "green";
        case Graphviz::Color::InTree: return "black";
        case Graphviz::Color::OutsideTree: return "grey";
        default:
            return "red";
    }
}

using NodeMap = std::unordered_map<Recipe *, Graphviz::Color>;

}

void Graphviz::process_(std::ostream & oss, const DependencyGraph & graph) const
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

    for(const auto & p : nodes)
        write_node_(oss, p.first, p.second);

    for(const auto & p : nodes)
        write_edges_(oss, p.first, graph.root_book());

    write_footer_(oss);
}

namespace  {

unsigned int to_int(Graphviz::Color c)
{
    switch(c)
    {
        case Graphviz::Color::Root: return 0;
        case Graphviz::Color::InTree: return 1;
        case Graphviz::Color::OutsideTree: return 2;
        default: return 3;
    }
}

Graphviz::ColorPropertyMap defaultColorPropertyMap(Graphviz::Color c)
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

}

Graphviz::Graphviz()
{
    for (Color c : {Color::Root, Color::InTree, Color::OutsideTree, Color::Undefined})
        color_map_[to_int(c)] = defaultColorPropertyMap(c);
}



const Graphviz::ColorPropertyMap & Graphviz::color_property_map(Color c) const
{
    return color_map_[to_int(c)];
}
Graphviz::ColorPropertyMap & Graphviz::color_property_map(Color c)
{
    return color_map_[to_int(c)];
}

std::string Graphviz::color_property_string(Color color) const
{
    const ColorPropertyMap & cp = color_property_map(color);

    std::ostringstream oss;
    for(auto it = cp.begin(); it != cp.end(); ++it)
    {
        if (it != cp.begin())
            oss << ", ";
        oss << it->first << "=\"" << it->second << "\"";
    }

    return oss.str();
}

void Graphviz::write_node_desc_(std::ostream & oss, const std::string & id, const std::string & uri, Color color) const
{
    oss << id << " [label=\"" << uri << "\", " << color_property_string(color) << "];" << std::endl;
}

void Graphviz::write_node_(std::ostream & oss, model::Recipe * recipe, Color color) const
{
    write_node_desc_(oss, node_name(recipe), recipe->uri().string(), color);
}
void Graphviz::write_edges_(std::ostream & oss, model::Recipe * recipe, Book *root) const
{
    const std::string & nd = node_name(recipe);

    for(const auto & p : recipe->dependency_pairs())
    {

        // make sure we try to resolve it
        Recipe * tgt = p.second;
        if (!tgt)
            algo::resolve_dependency(tgt, p.first, recipe->parent(), root);

        std::string tgt_id;

        if (!tgt)
        {
            tgt_id = unexisting_node_name(recipe, p.first);
            write_node_desc_(oss, tgt_id, p.first.string(), Graphviz::Color::Undefined);
        }
        else
        {
            tgt_id = node_name(tgt);
        }

        oss << nd << " -> " << tgt_id << ";" << std::endl;
    }
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
    const static std::string default_extension = "dot";
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
    return "dot";
}

std::shared_ptr<Interface> Graphviz::clone() const
{
    return std::make_shared<Graphviz>(*this);
}

Result Graphviz::set_option(const std::string & option)
{
    MSS_BEGIN(Result);

    filename = option;

    MSS_END();

}

bool Graphviz::can_process(const model::DependencyGraph & graph) const
{
    return true;
}

Result Graphviz::process(const model::DependencyGraph & graph, const model::Environment & environment)
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

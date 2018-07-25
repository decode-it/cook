#ifndef HEADER_cook_process_Menu_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_Menu_hpp_ALREADY_INCLUDED

#include "cook/process/build/Graph.hpp"
#include "cook/process/RecipeFilteredGraph.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/model/Book.hpp"
#include "cook/Log.hpp"
#include "gubg/graph/AdjacencyList.hpp"
#include <unordered_map>

namespace cook { namespace process {

class Menu
{
public:
    struct DependencyGraph
    {
        using Graph = gubg::graph::AdjacencyList<gubg::graph::use_vector, gubg::graph::use_list, gubg::graph::use_list, model::Recipe *, gubg::graph::no_label, gubg::graph::bidirectional>;
        using VertexDescriptor = gubg::graph::Traits<Graph>::vertex_descriptor;

        Graph graph;
        std::unordered_map<model::Recipe *, VertexDescriptor> translation_map;
    };

    struct ComponentGraph
    {
        using Graph = gubg::graph::AdjacencyList<gubg::graph::use_vector, gubg::graph::use_list, gubg::graph::use_list, std::set<model::Recipe*>, gubg::graph::no_label, gubg::graph::bidirectional>;
        using VertexDescriptor = gubg::graph::Traits<Graph>::vertex_descriptor;

        Graph graph;
        std::unordered_map<model::Recipe *, VertexDescriptor> translation_map;
    };

    Menu();

    template <typename It> Result construct(const gubg::Range<It> & range);

    bool is_valid() const;
    const std::list<model::Recipe*> & topological_order_recipes() const;
    Result topological_order_recipes(model::Recipe * root, std::list<model::Recipe*> & result) const;
    const std::list<build::GraphPtr> & topological_order_build_graphs() const;
    const std::list<model::Recipe*> & root_recipes() const;
    const DependencyGraph & dependency_graph() const;
    const ComponentGraph & component_graph() const;

    const RecipeFilteredGraph * recipe_filtered_graph(model::Recipe *recipe) const;
    RecipeFilteredGraph * recipe_filtered_graph(model::Recipe * recipe);

    void stream(log::Importance = log::Importance{}) const;

private:
    Menu(const Menu &) = delete;
    Menu(Menu &&) = delete;
    Menu & operator=(const Menu &) = delete;
    Menu & operator=(Menu &&) = delete;

    using CountMap = std::unordered_map<model::Recipe *, unsigned int>;
    Result construct_();

    std::list<model::Recipe *> topological_order_;
    std::list<model::Recipe *> root_recipes_;

    std::map<model::Recipe *, RecipeFilteredGraph> recipe_filtered_graphs_;
    std::list<build::GraphPtr> topological_build_graph_order_;
    bool valid_;

    DependencyGraph dependency_graph_;
    ComponentGraph component_graph_;
};

template <typename It>
Result Menu::construct(const gubg::Range<It> & range)
{
    root_recipes_.assign(RANGE(range));
    return construct_();
}


} }

#endif

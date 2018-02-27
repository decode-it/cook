#ifndef HEADER_cook_process_Menu_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_Menu_hpp_ALREADY_INCLUDED

#include "cook/process/FileDependencyGraph.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/model/Book.hpp"
#include <unordered_map>

namespace cook { namespace process {

class Menu
{
public:
    explicit Menu(model::Book * root_book);
    Result construct(const std::list<model::Recipe *> &roots);

private:
    Menu(const Menu &) = delete;
    Menu(Menu &&) = delete;
    Menu & operator=(const Menu &) = delete;
    Menu & operator=(Menu &&) = delete;

    using CountMap = std::unordered_map<model::Recipe *, unsigned int>;
    Result construct_count_map_(CountMap & in_degree_map, const std::list<model::Recipe*> & roots);
    bool initialize_process_info_();
    bool grow_(model::Recipe * seed, unsigned int component_identifier, const std::unordered_multimap<model::Recipe *, model::Recipe *> & in_edge_map);

    model::Book * root_;
    std::list<model::Recipe *> topological_order_;

    struct ProcessInfo
    {
        ProcessInfo(const model::Uri & uri): post(uri) {}

        std::shared_ptr<FileDependencyGraph> graph;
        std::vector<FileDependencyGraph::vertex_descriptor> commands;
        model::Snapshot post;
        unsigned int component_identifier = std::numeric_limits<unsigned int>::max();
    };

    std::map<model::Recipe *, ProcessInfo> process_info_map_;
};

} }

#endif

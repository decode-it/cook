#ifndef HEADER_cook_Menu_hpp_ALREADY_INCLUDED
#define HEADER_cook_Menu_hpp_ALREADY_INCLUDED

#include "cook/process/build/Graph.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/model/Book.hpp"
#include <unordered_map>

namespace cook { namespace process {

class Menu
{
public:
    Menu();

    template <typename It>
    Result construct(const gubg::Range<It> & range)
    {
        root_recipes_.assign(RANGE(range));
        return construct_();
    }


    bool is_valid() const { return valid_; }

    const std::list<model::Recipe*> & topological_order() const;
    const std::list<model::Recipe*> & root_recipes() const;

private:
    Menu(const Menu &) = delete;
    Menu(Menu &&) = delete;
    Menu & operator=(const Menu &) = delete;
    Menu & operator=(Menu &&) = delete;

    using CountMap = std::unordered_map<model::Recipe *, unsigned int>;
    Result construct_();
    Result construct_count_map_(CountMap & in_degree_map) const;
    Result construct_topological_order_(CountMap & in_degree_map);
    bool initialize_process_info_();
    bool grow_(model::Recipe * seed, unsigned int component_identifier, const std::unordered_multimap<model::Recipe *, model::Recipe *> & in_edge_map);

    std::list<model::Recipe *> topological_order_;
    std::list<model::Recipe *> root_recipes_;

    struct ProcessInfo
    {
        ProcessInfo(const model::Uri & uri): post(uri) {}

        std::shared_ptr<build::Graph> graph;
        std::vector<build::Graph::vertex_descriptor> commands;
        model::Snapshot post;
        unsigned int component_identifier = std::numeric_limits<unsigned int>::max();
    };

    std::map<model::Recipe *, ProcessInfo> process_info_map_;
    bool valid_;
};

} }

#endif

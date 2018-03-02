#ifndef HEADER_cook_process_chef_Chef_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_chef_Chef_hpp_ALREADY_INCLUDED

#include "cook/process/chef/Brigade.hpp"
#include "cook/process/Menu.hpp"
#include "cook/Context.hpp"
#include <map>

namespace cook { namespace process { namespace chef {

class Chef
{
public:
    virtual ~Chef() {}

    virtual Result initialize() = 0;

    template <typename Brigade>
    void add_brigade(unsigned int priority, Brigade && brigade)
    {
        brigade_priority_map_.emplace(priority, std::forward<Brigade>(brigade));
    }

    // I know it should mets en place, but this corresponds to the cooking term :)
    Result mis_en_place(Context & kitchen);

    Result find_brigade(const Brigade *& result, model::Recipe * recipe) const;

private:
    Result mis_en_place_(model::Recipe &recipe, RecipeFilteredGraph & file_command_graph, const Context & context, const Brigade &staff) const;

    std::multimap<unsigned int, Brigade> brigade_priority_map_;

};


} } }

#endif

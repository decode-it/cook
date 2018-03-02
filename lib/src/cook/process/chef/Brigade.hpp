#ifndef HEADER_cook_process_chef_Brigade_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_chef_Brigade_hpp_ALREADY_INCLUDED

#include "cook/process/souschef/Interface.hpp"
#include "cook/model/Recipe.hpp"
#include <functional>

namespace cook { namespace process { namespace chef {

using SouschefPtr = std::shared_ptr<souschef::Interface>;

struct Brigade
{
    std::string name;
    std::function< bool (const model::Recipe *, std::string & )> can_cook;
    std::list<SouschefPtr> souschefs;
};

} } }

#endif

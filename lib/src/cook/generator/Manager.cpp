#include "cook/generator/Manager.hpp"
#include "cook/generator/NaftWriter.hpp"
#include "cook/generator/NinjaWriter.hpp"

namespace cook { namespace generator {

Manager::Manager()
{
    generators_.insert(std::make_pair(cook_api_GenerationType_Ninja, std::make_shared<NinjaFactory>() ) );
    generators_.insert(std::make_pair(cook_api_GenerationType_Naft, std::make_shared<NaftFactory>() ) );
}

bool Manager::has_generator(cook_api_GenerationType_t key) const
{
    return generators_.count(key) > 0;
}

Manager::FactoryPtr Manager::find_generator(cook_api_GenerationType_t key) const
{
    auto it = generators_.find(key);
    if (it == generators_.end())
        return FactoryPtr();
    return it->second;
}

} }

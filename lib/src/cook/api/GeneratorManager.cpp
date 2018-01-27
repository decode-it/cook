#include "cook/api/GeneratorManager.h"
#include "cook/api/Element.hpp"
#include "cook/generator/Manager.hpp"
#include "cook/API.hpp"

using namespace cook::api;

cook_bool_t cook_api_GeneratorManager_generate(cook_api_Element_t element, cook_api_GenerationType_t type, int argc, const char * argv[])
{
    MSS_BEGIN(bool);

    auto & mgr = get_manager(element);
    auto & api = get_api(element);

    cook::generator::Manager::FactoryPtr ptr = mgr.find_generator(type);
    MSS(!!ptr, LOG(api, Error, "Unable to find the requested generator for " << type));

    std::vector<std::string> args(argc);
    for(unsigned int i = 0; i < argc; ++i)
        args[i] = argv[i];

    auto gen = ptr->construct(api);
    MSS(!!gen);
    MSS(!!gen->visit(api, args));
    MSS(!!gen->process());


    MSS_END();
}

#include "cook/API.hpp"
#include "gubg/mss.hpp"
#include "cook/api/Element.hpp"

namespace cook {

API::API(const cook_API_Parameters_t & parameters)
    : Logger(parameters.callback, parameters.callback_context),
      project_dir_(parameters.project_directory)
{
    env.set_project_dir(project_dir_);
}

std::filesystem::path API::resolve_local(const std::string & fod) const
{
    return gubg::filesystem::get_relative_to(project_dir_, fod);
}

}

namespace {

bool valid_parameters(const cook_API_Parameters_t * parameters)
{
    MSS_BEGIN(bool);

    MSS(!!parameters);
    MSS(!!parameters->callback);
    MSS(!!parameters->project_directory);

    MSS_END();
}

}


cook_Handle_t cook_API_create(const cook_API_Parameters_t * params)
{
    if (!valid_parameters(params))
        return nullptr;

    std::unique_ptr<cook::API> api = std::make_unique<cook::API>(*params);
    return api.release();

}
void cook_API_destroy(cook_Handle_t api)
{
    if (api)
        delete static_cast<cook::API *>(api);
}

cook_api_Element_t cook_API_root_book(cook_Handle_t handle)
{
    cook::API * api = static_cast<cook::API *>(handle);
    return cook::api::make_element(*api, api->library.root());
}

cook_api_Element_t cook_API_generator_manager(cook_Handle_t handle)
{
    cook::API * api = static_cast<cook::API *>(handle);
    return cook::api::make_element(*api, api->mgr);
}

void cook_API_set_toolchain_name(cook_Handle_t handle, const char * name)
{
    cook::API * api = static_cast<cook::API *>(handle);
    api->toolchain.set_name(name);
}
void cook_API_set_toolchain_config(cook_Handle_t handle, const char * config)
{
    cook::API * api = static_cast<cook::API *>(handle);
    api->toolchain.set_config(config);
}
void cook_API_set_toolchain_arch(cook_Handle_t handle, const char * arch)
{
    cook::API * api = static_cast<cook::API *>(handle);
    api->toolchain.set_arch(arch);
}

void cook_API_set_cache_dir(cook_Handle_t handle, const char * dir)
{
    cook::API * api = static_cast<cook::API *>(handle);
    api->env.set_temp_dir(dir);
}
void cook_API_set_output_dir(cook_Handle_t handle, const char * dir)
{
    cook::API * api = static_cast<cook::API *>(handle);
    api->env.set_output_dir(dir);
}

cook_bool_t cook_API_resolve(cook_Handle_t handle)
{
    cook::API * api = static_cast<cook::API *>(handle);
    return api->library.resolve();
}

#include "cook/chai/module/Toolchain.hpp"
#include "cook/chai/Toolchain.hpp"
#include "cook/chai/Recipe.hpp"
#include "gubg/chai/Module.hpp"

namespace cook { namespace chai { namespace module {

    gubg::chai::ModulePtr toolchain()
    {
        auto ptr = gubg::chai::make_module();
        using CFGBoard = Toolchain::ConfigurationBoard;

        ptr->add(chaiscript::user_type<CFGBoard>(), "ConfigurationBoard");
        ptr->add(chaiscript::fun([](CFGBoard & b, const std::string & k) { b.add_configuration(k); }), "add_config");
        ptr->add(chaiscript::fun([](CFGBoard & b, const std::string & k, const std::string & v) { b.add_configuration(k, v); }), "add_config");


        ptr->add(chaiscript::user_type<Toolchain::ConfigurationCallback>(), "ConfigurationCallback");
        ptr->add(chaiscript::user_type<Toolchain>(), "Toolchain");
        ptr->add(chaiscript::fun(&Toolchain::has_element), "has_element");
        ptr->add(chaiscript::fun(&Toolchain::element), "element");
        ptr->add(chaiscript::fun(&Toolchain::add_config_1), "add_config");
        ptr->add(chaiscript::fun(&Toolchain::add_config_2), "add_config");
        ptr->add(chaiscript::fun(&Toolchain::configure), "configure");
        ptr->add(chaiscript::fun(&Toolchain::each_config_1), "each_config");
        ptr->add(chaiscript::fun(&Toolchain::each_config_2), "each_config");

        ptr->add(chaiscript::fun(&Toolchain::has_config_1), "has_config");
        ptr->add(chaiscript::fun(&Toolchain::has_config_2), "has_config");

        ptr->add(chaiscript::user_type<Toolchain::PrimaryNameFunctor>(), "PrimaryNameFunctor");
        ptr->add(chaiscript::fun(&Toolchain::set_primary_name_functor), "set_target_namer");

        return ptr;
    }

} } }

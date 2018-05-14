#include "cook/chai/module/Cook.hpp"
#include "cook/chai/Cook.hpp"
#include "gubg/chai/Module.hpp"

namespace cook { namespace chai { namespace module {

    gubg::chai::ModulePtr cook()
    {
        auto ptr = gubg::chai::make_module();

        ptr->add(chaiscript::fun(&Cook::operator[]), "[]");
        ptr->add(chaiscript::fun([](const Cook & c) {return c.working_directory(); }), "working_directory");
        ptr->add(chaiscript::fun([](const Cook & c, bool abs) {return c.working_directory(abs); }), "working_directory");
        ptr->add(chaiscript::fun(&Cook::project_name), "project_name");
        ptr->add(chaiscript::fun(&Cook::set_project_name), "set_project_name");
        ptr->add(chaiscript::fun(&Cook::toolchain), "toolchain");

        return ptr;
    }

} } }


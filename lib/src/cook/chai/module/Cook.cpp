#include "cook/chai/module/Cook.hpp"
#include "cook/chai/Cook.hpp"
#include "gubg/chai/Module.hpp"

namespace cook { namespace chai { namespace module {

    gubg::chai::ModulePtr cook()
    {
        auto ptr = gubg::chai::make_module();

        ptr->add(chaiscript::fun(&Cook::operator[]), "[]");
        ptr->add(chaiscript::base_class<Book, Cook>());
        ptr->add(chaiscript::fun([](const Cook & c) {return c.script_local_directory(); }), "script_local_directory");
        ptr->add(chaiscript::fun(&Cook::project_name), "project_name");
        ptr->add(chaiscript::fun(&Cook::set_project_name), "set_project_name");
        ptr->add(chaiscript::fun(&Cook::toolchain), "toolchain");
        ptr->add(chaiscript::fun([](const Cook & c) {return c.output_directory(); }), "output_directory");
        ptr->add(chaiscript::fun([](const Cook & c, bool abs) {return c.output_directory(abs); }), "output_directory");
        ptr->add(chaiscript::fun([](const Cook & c) {return c.temporary_directory(); }), "temporary_directory");
        ptr->add(chaiscript::fun([](const Cook & c, bool abs) {return c.temporary_directory(abs); }), "temporary_directory");

        return ptr;
    }

} } }


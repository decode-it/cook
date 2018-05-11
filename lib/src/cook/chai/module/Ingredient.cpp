#include "cook/chai/module/Ingredient.hpp"
#include "cook/chai/File.hpp"
#include "cook/chai/KeyValue.hpp"
#include "gubg/chai/Module.hpp"

namespace cook { namespace chai { namespace module {

    namespace {

        template <typename T>
        void expose_basic(gubg::chai::ModulePtr & ptr)
        {
            ptr->add(chaiscript::fun([](T & t) -> Flags & { return t.flags(); }),               "flags");
            ptr->add(chaiscript::fun([](const T & t) -> const Flags & { return t.flags();  }),  "flags");
            ptr->add(chaiscript::fun([](const T & t) { return t.has_owner(); }),                "has_owner");
            ptr->add(chaiscript::fun([](const T & t) { return t.owner(); }),                    "owner");
            ptr->add(chaiscript::fun(&T::key),                                                  "key");
            ptr->add(chaiscript::fun(&T::is_file),                                              "is_file");
            ptr->add(chaiscript::fun(&T::is_key_value),                                         "is_key_value");
        }

    }

    gubg::chai::ModulePtr ingredient()
    {
        auto ptr = gubg::chai::make_module();

        ptr->add(chaiscript::user_type<File>(), "File");
        expose_basic<File>(ptr);
        ptr->add(chaiscript::fun(&File::dir), "dir");
        ptr->add(chaiscript::fun(&File::rel), "rel");


        ptr->add(chaiscript::user_type<KeyValue>(), "KeyValue");
        expose_basic<KeyValue>(ptr);
        ptr->add(chaiscript::fun(&KeyValue::has_value), "has_value");
        ptr->add(chaiscript::fun(&KeyValue::value), "value");

        return ptr;
    }

} } }

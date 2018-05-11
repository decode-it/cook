#include "cook/chai/module/Basic.hpp"
#include "cook/chai/mss.hpp"
#include "gubg/chai/Module.hpp"

namespace cook { namespace chai { namespace module { 

    namespace {

        void assert_true_1(bool val)
        {
            CHAI_MSS_BEGIN();
            CHAI_MSS(val);
        }

        void assert_true_2(bool val, const std::string & message)
        {
            CHAI_MSS_BEGIN();
            CHAI_MSS_MSG(val, Error, message);
        }
    }

    gubg::chai::ModulePtr basic()
    {
        auto ptr = gubg::chai::make_module();

        ptr->add(chaiscript::fun(&assert_true_1), "assert");
        ptr->add(chaiscript::fun(&assert_true_2), "assert");

        ptr->eval("def assert_equal(a,b) { assert(a==b, \"${a} is not equal to ${b}\") }");
        ptr->eval("def assert_not_equal(a,b) { assert(a!=b, \"${a} is equal to ${b}\") }");

        return ptr;
    }

} } }

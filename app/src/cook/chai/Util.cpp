#include "cook/chai/Util.hpp"
#include "gubg/chai/Module.hpp"

namespace cook { namespace chai {

gubg::chai::ModulePtr util_module()
{
    gubg::chai::ModulePtr m(new chaiscript::Module());

    return m;
}

} }



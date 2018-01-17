#include "cook/view/chai/Util.hpp"
#include "chaiscript/chaiscript.hpp"

namespace cook { namespace view { namespace chai {

ModulePtr util_module()
{
    ModulePtr m(new chaiscript::Module());

    return m;
}

} } }



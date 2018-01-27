#include "cook/view/chai/Flags.hpp"
#include "cook/Flags.h"
#include "chaiscript/chaiscript.hpp"

namespace cook { namespace view { namespace chai {

ModulePtr flags_module()
{
    ModulePtr m(new chaiscript::Module());

    chaiscript::utility::add_class<cook_Flags_t>(*m,
     "Flags",
    { { cook_Flags_Overwrite_Never, "overwrite_never" },
      { cook_Flags_Overwrite_IfSame, "overwrite_if_same" },
      { cook_Flags_Overwrite_Always, "overwrite_always" },
      { cook_Flags_Propagation_Public, "propagation_public" },
      { cook_Flags_Propagation_Private, "propagation_private" },
      { cook_Flags_ForceInclude, "force_include" } } );

    return m;
}

} } }

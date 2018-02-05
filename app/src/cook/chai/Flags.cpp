#include "cook/chai/Flags.hpp"
#include "cook/Flags.h"
#include "gubg/chai/Module.hpp"

namespace cook { namespace chai {

gubg::chai::ModulePtr flags_module()
{
    gubg::chai::ModulePtr m(new chaiscript::Module());

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

} }

#ifndef HEADER_cook_chai_module_RaiiIngredient_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_module_RaiiIngredient_hpp_ALREADY_INCLUDED

#include "cook/chai/RaiiIngredient.hpp"

namespace chaiscript
{
    class Boxed_Value;
}

namespace cook { namespace chai { namespace module {

    chaiscript::Boxed_Value as_boxed(RaiiFile & file);
    chaiscript::Boxed_Value as_boxed(RaiiKeyValue & key_value);


} } }

#endif


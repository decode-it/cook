#include "cook/chai/module/RaiiIngredient.hpp"
#include "chaiscript/dispatchkit/boxed_value.hpp"

namespace cook { namespace chai { namespace module {

    chaiscript::Boxed_Value as_boxed(RaiiFile & file)
    {
        return chaiscript::var(&file.chai);
    }

    chaiscript::Boxed_Value as_boxed(RaiiKeyValue & key_value)
    {
        return chaiscript::var(&key_value.chai);
    }

} } }

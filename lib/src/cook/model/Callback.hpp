#ifndef HEADER_cook_model_Callback_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Callback_hpp_ALREADY_INCLUDED

#include "cook/Hook.hpp"
#include <functional>
#include <array>

namespace cook { namespace model {

    class Recipe;

    using Callback = std::function<void (Recipe &)>;
    using Callbacks = std::array<Callback, static_cast<std::size_t>(Hook::_End)>;

} }

#endif


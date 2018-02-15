#include "cook/model/Recipe.hpp"

namespace cook { namespace model {

Recipe::Recipe(const Uri & uri)
    : Snapshot(uri),
      post_(uri)
{
}

} }


#include "cook/log/Root.hpp"

namespace cook { namespace log {

Root::Root(std::ostream & oss)
    : gubg::naft::Document(oss),
      Scope(0, *this)
{

}

} }


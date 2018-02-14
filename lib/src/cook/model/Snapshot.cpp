#include "cook/model/Snapshot.hpp"

namespace cook { namespace model {

bool Snapshot::set_uri(const Uri & uri)
{
    MSS_BEGIN(bool);
    MSS(uri.has_name());

    uri_ = uri;

    MSS_END();
}


} }


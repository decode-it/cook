#include "cook/model/Snapshot.hpp"

namespace cook { namespace model {

Snapshot::Snapshot(const Uri & uri)
    : uri_(uri)
{
}

const Uri & Snapshot::uri() const
{
    return uri_;
}

bool Snapshot::set_uri(const Uri & uri)
{
    MSS_BEGIN(bool);

    MSS(uri.has_name());
    uri_ = uri;

    MSS_END();
}

const std::filesystem::path & Snapshot::working_directory() const
{
    return wd_;
}

void Snapshot::set_working_directory(const std::filesystem::path & wd)
{
    wd_ = wd;
}


} }


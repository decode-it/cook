#include "cook/model/Snapshot.hpp"

namespace cook { namespace model {

Snapshot::Snapshot(const Uri & uri)
    : uri_(uri)
{

}

bool Snapshot::add_property(const property::PropertiesKey & key, const property::File & file)
{
    return file_properties_.add(key, file);
}

bool Snapshot::add_property(const property::PropertiesKey & key, const property::KeyValue & key_value)
{
    return key_value_properties_.add(key, key_value);
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


} }


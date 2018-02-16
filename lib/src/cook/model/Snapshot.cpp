#include "cook/model/Snapshot.hpp"

namespace cook { namespace model {

Snapshot::Snapshot(const Uri & uri)
    : uri_(uri)
{

}

bool Snapshot::insert_or_merge(const LanguageTypePair &key, const property::File & file)
{
    return file_properties_.add_or_merge(key, file);
}

bool Snapshot::insert_or_merge(const LanguageTypePair &key, const property::KeyValue & key_value)
{
    return key_value_properties_.add_or_merge(key, key_value);
}

std::pair<Snapshot::FileIterator, bool> Snapshot::insert(const LanguageTypePair & collection_key, const property::File & property)
{
    return file_properties_.insert(collection_key, property);
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


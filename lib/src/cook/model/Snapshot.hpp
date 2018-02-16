#ifndef HEADER_cook_model_Snapshot_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Snapshot_hpp_ALREADY_INCLUDED

#include "cook/property/Properties.hpp"
#include "cook/property/File.hpp"
#include "cook/property/KeyValue.hpp"
#include "cook/model/Uri.hpp"

namespace cook { namespace model {

class Snapshot
{
public:
    Snapshot(const Uri & uri);

    bool add_property(const LanguageTypePair & key, const property::File & file);
    bool add_property(const LanguageTypePair & key, const property::KeyValue & key_value);

    const Uri & uri() const;
    bool set_uri(const Uri & uri);
    const std::filesystem::path & working_directory() const;

private:
    property::Properties<property::File> file_properties_;
    property::Properties<property::KeyValue> key_value_properties_;
    Uri uri_;
    std::filesystem::path wd_;
};


} }

#endif

#ifndef HEADER_cook_model_Snapshot_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Snapshot_hpp_ALREADY_INCLUDED

#include "cook/model/property/Properties.hpp"
#include "cook/model/property/File.hpp"
#include "cook/model/property/KeyValue.hpp"
#include "cook/model/Uri.hpp"

namespace cook { namespace model {

class Snapshot
{
public:
    bool add_property(const property::PropertiesKey & key, const property::File & file)             { return file_properties_.add(key, file); }
    bool add_property(const property::PropertiesKey & key, const property::KeyValue & key_value)    { return key_value_properties_.add(key, key_value); }

    const Uri & uri() const         { return uri_; }
    Uri & uri()                     { return uri_; }
    bool set_uri(const Uri & uri);
    const std::filesystem::path & working_directory() const { return wd_; }

private:
    property::Properties<property::File> file_properties_;
    property::Properties<property::KeyValue> key_value_properties_;
    Uri uri_;
    std::filesystem::path wd_;
};


} }

#endif

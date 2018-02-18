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
    using FileProperties = property::Properties<property::File>;
    using KeyValueProperties = property::Properties<property::KeyValue>;
    Snapshot(const Uri & uri);

    const FileProperties & file_properties() const          { return file_properties_; }
    const KeyValueProperties & key_value_properties() const { return key_value_properties_; }
    FileProperties & file_properties()                      { return file_properties_; }
    KeyValueProperties & key_value_properties()             { return key_value_properties_; }

    const Uri & uri() const;
    bool set_uri(const Uri & uri);
    const std::filesystem::path & working_directory() const;
    void set_working_directory(const std::filesystem::path & wd);

private:
    FileProperties file_properties_;
    KeyValueProperties key_value_properties_;
    Uri uri_;
    std::filesystem::path wd_;
};


} }

#endif

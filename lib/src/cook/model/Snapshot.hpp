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
    using FileIterator = property::Collection<property::File>::iterator;

    Snapshot(const Uri & uri);

    bool insert_or_merge(const LanguageTypePair & key, const property::File & file);
    bool insert_or_merge(const LanguageTypePair & key, const property::KeyValue & key_value);

    std::pair<FileIterator, bool> insert(const LanguageTypePair & collection_key, const property::File & property);

    template <typename Functor> bool each_file(Functor && functor) const
    {
        return file_properties_.each(std::forward<Functor>(functor));
    }

    property::File * find(const LanguageTypePair & collection_key, const typename property::File::key_type & property_key) { return file_properties_.find(collection_key, property_key); }
    const property::File * find(const LanguageTypePair & collection_key, const typename property::File::key_type & property_key) const { return file_properties_.find(collection_key, property_key); }


    const Uri & uri() const;
    bool set_uri(const Uri & uri);
    const std::filesystem::path & working_directory() const;
    void set_working_directory(const std::filesystem::path & wd);

private:
    property::Properties<property::File> file_properties_;
    property::Properties<property::KeyValue> key_value_properties_;
    Uri uri_;
    std::filesystem::path wd_;
};


} }

#endif

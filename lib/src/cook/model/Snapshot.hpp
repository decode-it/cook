#ifndef HEADER_cook_model_Snapshot_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Snapshot_hpp_ALREADY_INCLUDED

#include "cook/ingredient/Properties.hpp"
#include "cook/ingredient/File.hpp"
#include "cook/ingredient/KeyValue.hpp"
#include "cook/model/Uri.hpp"

namespace cook { namespace model {

class Snapshot
{
public:
    using Files = ingredient::Ingredients<ingredient::File>;
    using KeyValues = ingredient::Ingredients<ingredient::KeyValue>;
    Snapshot(const Uri & uri);

    const Files & files() const          { return files_; }
    const KeyValues & key_values() const { return key_values_; }
    Files & files()                      { return files_; }
    KeyValues & key_values()             { return key_values_; }

    const Uri & uri() const;
    bool set_uri(const Uri & uri);
    const std::filesystem::path & working_directory() const;
    void set_working_directory(const std::filesystem::path & wd);

private:
    Files files_;
    KeyValues key_values_;
    Uri uri_;
    std::filesystem::path wd_;
};


} }

#endif

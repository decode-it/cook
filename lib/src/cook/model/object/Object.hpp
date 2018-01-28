#ifndef HEADER_cook_model_object_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_object_Interface_hpp_ALREADY_INCLUDED

#include "cook/Flags.h"
#include <string>

namespace cook { namespace model {

struct Owner
{
    enum Type {Nobody = 0x00, Me = 0x01, Deps = 0x02, External = 0x04, Anybody = 0x07};
};


namespace object {

enum class Type
{
    Header,
    Source,
    UnknownFile,
    ForceInclude,
    Define,
    Library,
    LibraryPath,
    IncludePath
};

class Object
{
public:
    Object(Type type, const std::string & key) : type_(type), key_(key) {}

    const std::string & key() const     { return key_; }
    Type type() const                   { return type_; }


    cook_Propagation_t propagation = cook_Propagation_Default;
    Owner::Type owner = Owner::Me;

protected:
    bool equal_(const Object & rhs) const
    {
        return type() == rhs.type() && propagation == rhs.propagation && owner == rhs.owner;
    }

    void copy_data(const Object & rhs)
    {
        propagation = rhs.propagation;
        owner = rhs.owner;
    }

    void set_key(const std::string & key) { key_ = key; }

private:
    Type type_;
    std::string key_;
};

} } }

#endif

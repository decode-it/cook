#ifndef HEADER_cook_model_object_Define_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_object_Define_hpp_ALREADY_INCLUDED

#include "cook/model/object/Object.hpp"

namespace cook { namespace model { namespace object {

class Define :public Object
{
public:
    Define(const std::string & name);
    Define(const std::string & name, const std::string & value);

    bool operator==(const Define & rhs) const;

    bool has_value() const;

    std::string name;
    std::string value;
};

} } }

#endif

#ifndef HEADER_cook_model_Element_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Element_hpp_ALREADY_INCLUDED

#include "cook/model/Uri.hpp"
#include <any>

namespace cook { namespace model {

struct Book;

class Element
{
public:
    explicit Element(const Uri & uri);

    const Uri & uri() const;
    Book * parent() const;

    std::any & user_data();
    const std::any & user_data() const;

protected:
    bool set_parent(Book * parent);

private:
    Uri uri_;
    Book * parent_;
    std::any user_data_;

};


} }

#endif

#ifndef HEADER_cook_chai_UserData_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_UserData_hpp_ALREADY_INCLUDED

#include <memory>
#include <string>
#include "gubg/chai/ModuleFwd.hpp"

namespace cook { namespace chai {

class UserData
{
    struct D;

public:
    UserData();
    ~UserData();

    void set_variable(const std::string & name, const std::string & value);
    bool operator==(const UserData & rhs) const;

private:
    friend gubg::chai::ModulePtr user_data_module();

    std::shared_ptr<D> d_;
};

gubg::chai::ModulePtr user_data_module();

} }

#endif

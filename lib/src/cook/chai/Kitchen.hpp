#ifndef HEADER_cook_chai_Kitchen_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Kitchen_hpp_ALREADY_INCLUDED

#include "cook/Kitchen.hpp"

namespace cook { namespace chai {

struct Kitchen : public cook::Kitchen
{
    Kitchen();
    virtual ~Kitchen();

    bool add_variables(const std::list<Variable> & variables) override;
    cook::Logger & logger() override;

    bool load(const std::list<std::string> & recipes);

    const model::Book & root() const { return root_; }

private:
    void include_(const std::string & file);
    void reset_engine_();
    std::filesystem::path generate_file_path_(const std::string & file) const;

    struct D;
    model::Book root_;
    std::unique_ptr<D> d;
};

} }

#endif


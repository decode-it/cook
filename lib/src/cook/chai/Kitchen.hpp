#ifndef HEADER_cook_chai_Kitchen_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Kitchen_hpp_ALREADY_INCLUDED

#include "cook/Kitchen.hpp"

namespace cook { namespace chai {

struct Kitchen : public cook::Kitchen
{
    Kitchen();
    virtual ~Kitchen();

    cook::Logger & logger() override;

    bool load(const std::string &recipe);

private:
    std::shared_ptr<model::Environment> create_environment() const override;

    void include_(const std::string & file);
    void reset_engine_();
    std::filesystem::path generate_file_path_(const std::string & file) const;

    struct D;
    std::unique_ptr<D> d;
};

} }

#endif


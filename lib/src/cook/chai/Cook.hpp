#ifndef HEADER_cook_chai_Cook_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Cook_hpp_ALREADY_INCLUDED

#include "cook/chai/Book.hpp"
#include "cook/chai/Toolchain.hpp"

namespace cook {

    class Context;

}

namespace cook { namespace chai {

    class Cook
    {
    public:
        Cook(model::Book * root_book, Context * context);

        Book operator[](const model::Uri & uri);
        Book root() const;
        std::string working_directory() const;
        std::string working_directory(bool make_absolute) const;

        std::string project_name() const;
        void set_project_name(const std::string & name);
        Toolchain toolchain() const;

    private:
        Book root_;
        Context * context_;
        Toolchain toolchain_;
    };

} }

#endif


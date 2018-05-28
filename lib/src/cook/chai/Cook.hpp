#ifndef HEADER_cook_chai_Cook_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Cook_hpp_ALREADY_INCLUDED

#include "cook/chai/Book.hpp"
#include "cook/chai/Toolchain.hpp"

namespace cook {

    class Context;

}

namespace cook { namespace chai {

    class Cook : public Book
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

        std::string project_directory() const;
        std::string output_directory() const;
        std::string temporary_directory() const;
        std::string project_directory(bool make_absolute) const;
        std::string output_directory(bool make_absolute) const;
        std::string temporary_directory(bool make_absolute) const;


    private:
        Context * context_;
        Toolchain toolchain_;
    };

} }

#endif


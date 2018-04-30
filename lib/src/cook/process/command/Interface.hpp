#ifndef HEADER_cook_process_command_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_Interface_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "gubg/std/filesystem.hpp"
#include <string>
#include <memory>

namespace cook { namespace process { namespace command {

class Interface
{
public:
    enum Type {
        Compile,
        Archive,
        Link,
        UserDefined
    };

    virtual ~Interface() {}

    virtual std::string name() const = 0;
    virtual Result process(const std::list<std::filesystem::path> & input_files, const std::list<std::filesystem::path> & output_files) = 0;
    virtual void stream_command(std::ostream & oss, const std::list<std::filesystem::path> & input_files, const std::list<std::filesystem::path> & output_files) const = 0;
    virtual std::string depfile(const std::list<std::filesystem::path> & input_files, const std::list<std::filesystem::path> & output_files) const {return "";}
    virtual Type type() const = 0;

    virtual void set_recipe_uri(const std::string &uri) {recipe_uri_ = uri;}
    virtual const std::string &recipe_uri() const {return recipe_uri_;}

private:
    std::string recipe_uri_;
};

inline std::ostream & operator<<(std::ostream & str, Interface::Type type)
{
    switch(type)
    {
#define L_CASE(TYPE, STR) case Interface::TYPE: return str << STR
        L_CASE(Compile, "compile");
        L_CASE(Archive, "archive");
        L_CASE(Link, "link");
#undef L_CASE

        default:
            break;
    }

    if (type >= Interface::UserDefined)
        return str << "userdefined_" << static_cast<unsigned int>(type);
    else
        return str << "unknown";

}

using Ptr = std::shared_ptr<Interface>;

} } }

#endif

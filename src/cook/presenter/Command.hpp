#ifndef HEADER_cook_presenter_Command_hpp_ALREADY_INCLUDED
#define HEADER_cook_presenter_Command_hpp_ALREADY_INCLUDED

#include <string>

namespace cook { namespace presenter {

enum class Command
{
    script,
    filename,
    help,
    message,
    env,
    dir,
    build,
    output,
    toolchain,
    name,
    config,
    arch,
    model,
    book,
    recipe,
    create,
    add,
    define,
    action,
    generate,
    ninja,
    details,
    structure,
    display_name,
    depends_on,
    library,
    library_path,
    print,
    books,
    runner,
    done
};


inline std::string hr(Command c)
{
    switch(c)
    {
#define LC(NAME) case Command::NAME: return #NAME
        LC(script);
        LC(filename);
        LC(help);
        LC(message);
        LC(env);
        LC(dir);
        LC(build);
        LC(output);
        LC(toolchain);
        LC(name);
        LC(config);
        LC(arch);
        LC(model);
        LC(book);
        LC(recipe);
        LC(create);
        LC(add);
        LC(define);
        LC(action);
        LC(generate);
        LC(ninja);
        LC(details);
        LC(structure);
        LC(display_name);
        LC(depends_on);
        LC(library);
        LC(library_path);
        LC(print);
        LC(books);
        LC(runner);
        LC(done);
#undef LC
        default:
            return "unknown";
    }
};


} }

#endif

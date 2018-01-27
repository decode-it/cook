#include "cook/view/chai/File.hpp"
#include "cook/view/File.hpp"
#include "chaiscript/chaiscript.hpp"

namespace cook { namespace view { namespace chai {

ModulePtr file_module()
{
    ModulePtr m(new chaiscript::Module());

    chaiscript::utility::add_class<cook_FileType_t>(*m,
          "FileType",
          { { cook_FileType_Header, "header" }, { cook_FileType_Source, "source" }, { cook_FileType_Unknown, "unkown" } }
          );

    m->add(chaiscript::user_type<Path>(), "Path");
    m->add(chaiscript::fun(&Path::component), "component");
    m->add(chaiscript::fun(&Path::num_components), "size");
    m->add(chaiscript::fun(&Path::has_filename), "has_filename");

    m->add(chaiscript::user_type<File>(), "File");
    m->add(chaiscript::fun(&File::directory_path), "directory");
    m->add(chaiscript::fun(&File::relative_path), "relative");
    m->add(chaiscript::fun(&File::flags), "flags");
    m->add(chaiscript::fun(&File::type), "type");
    m->add(chaiscript::fun(&File::set_flags), "set_flags");
    m->add(chaiscript::fun(&File::set_type), "set_type");

    return m;
}

} } }

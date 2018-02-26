#include "cook/staff/Archiver.hpp"
#include "boost/predef.h"
#include "gubg/stream.hpp"

namespace cook { namespace staff {

namespace  {


std::string construct_archive_filename(const Context & context)
{
#if BOOST_OS_WINDOWS
    return gubg::stream([&](auto & os) {os << << context.recipe->uri().string('.') << ".lib"; });
#else
    return gubg::stream([&](auto & os) {os << "lib" << context.recipe->uri().string('.') << ".a"; });
#endif
}

}

Result Archiver::process(const Context & context, model::Snapshot & snapshot) const
{
    MSS_BEGIN(Result);

    model::Snapshot::Files & files = snapshot.files();

    auto it = files.find(LanguageTypePair(Language::ObjectCode, Type::Object));
    if (it == files.end())
    {
        MSS_RC << MESSAGE(Warning, "archive for " << snapshot.uri() << " is not created as it contains no object code");
        MSS_RETURN_OK();
    }

    // stop the propagation, this file is contained in the library
    for(ingredient::File & object : it->second)
        object.set_propagation(Propagation::Private);

    // create the archive
    const ingredient::File archive = construct_archive_file(context);
    const LanguageTypePair key(Language::ObjectCode, Type::Library);
    MSG_MSS(files.insert(key,archive).second, Error, "Archive " << archive << " already present in " << snapshot.uri());

    MSS_END();
}


ingredient::File Archiver::construct_archive_file(const Context &context) const
{
    const std::filesystem::path dir = context.environment->dirs.output() / context.recipe->pre().working_directory();
    const std::filesystem::path rel = construct_archive_filename(context);

    ingredient::File archive(dir, rel);
    archive.set_overwrite(Overwrite::IfSame);
    archive.set_owner(context.recipe);
    archive.set_propagation(Propagation::Public);

    return archive;
}

} }

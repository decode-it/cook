#include "cook/staff/Compiler.hpp"

namespace cook { namespace staff {

Compiler::Compiler(Language language)
    : language_(language)
{

}

Result Compiler::process(const Context & context, model::Snapshot & snapshot) const
{
    MSS_BEGIN(Result);

    model::Snapshot::Files & files = snapshot.files();

    auto it = files.find(LanguageTypePair(language_, Type::Source));
    if (it == files.end())
        MSS_RETURN_OK();

    for (const ingredient::File & source : it->second)
    {
        MSG_MSS(source.propagation() == Propagation::Private, Warning, "Source file '" << source << "' in " << snapshot.uri() << " has public propagation and will (probably) result into multiple defined symbols");

        const ingredient::File & object = construct_object_file(source, context);
        const LanguageTypePair key(Language::ObjectCode, Type::Object);

        MSG_MSS(files.insert(key, object).second, Error, "Object file '" << object << "' already present in " << snapshot.uri());
    }

    MSS_END();
}

ingredient::File Compiler::construct_object_file(const ingredient::File & source, const Context & context) const
{
    const std::filesystem::path dir = gubg::filesystem::combine(context.environment->dirs.temporary(), source.dir());
    const std::filesystem::path rel = source.rel().string() + ".obj";

    ingredient::File object(dir, rel);
    object.set_owner(context.recipe);
    object.set_overwrite(Overwrite::Never);
    object.set_propagation(Propagation::Public);

    return object;
}

} }

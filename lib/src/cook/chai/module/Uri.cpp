#include "cook/chai/module/Uri.hpp"
#include "cook/model/Uri.hpp"
#include "gubg/chai/Module.hpp"

namespace cook { namespace chai { namespace module { 

        using Uri = model::Uri;

    namespace {

        Uri combine(const Uri & lhs, const Uri & rhs) { return lhs / rhs; }
        Uri & combine_assign(Uri & lhs, const Uri & rhs) { return lhs /= rhs; }
        Uri & assign(Uri & lhs, const Uri & rhs) { return lhs = rhs; }
    }

    gubg::chai::ModulePtr uri()
    {
        auto ptr = gubg::chai::make_module();

        ptr->add(chaiscript::user_type<Uri>(), "Uri");
        ptr->add(chaiscript::constructor<Uri()>(), "Uri");
        ptr->add(chaiscript::constructor<Uri(const Uri &)>(), "Uri");
        ptr->add(chaiscript::type_conversion<std::string, Uri>());

        ptr->add(chaiscript::fun([](const model::Uri & uri) { return uri.string(); }), "to_string");
        ptr->add(chaiscript::fun([](const model::Uri & uri, char c) { return uri.string(c); }), "to_string");
        ptr->add(chaiscript::fun([](const model::Uri & uri, bool initial_separator) { return uri.string(initial_separator); }), "to_string");
        ptr->add(chaiscript::fun([](const model::Uri & uri, bool initial_separator, char c) { return uri.string(initial_separator, c); }), "to_string");

        ptr->add(chaiscript::fun(&combine), "/");
        ptr->add(chaiscript::fun(&combine_assign), "/=");
        ptr->add(chaiscript::fun(&assign), "=");
        ptr->add(chaiscript::fun(&Uri::operator==), "==");
        ptr->add(chaiscript::fun(&Uri::operator!=), "!=");
        
        ptr->add(chaiscript::fun(&model::Uri::as_absolute), "as_absolute");
        ptr->add(chaiscript::fun(&model::Uri::as_relative), "as_relative");
        
        return ptr;
    }

} } }

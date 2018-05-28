#include "cook/chai/module/Flags.hpp"
#include "cook/chai/Flags.hpp"
#include "gubg/chai/Module.hpp"

namespace cook { namespace chai { namespace module {

    namespace wrapper {

        namespace {

        struct Propagation_t {};
        struct Overwrite_t {};
        struct Type_t
	{
            Flags make_UserDefined(unsigned int v) const { return Flags(static_cast<Type>(static_cast<unsigned int>(Type::UserDefined) + v)); }
	};
        struct Language_t 
        {
            Flags make_UserDefined(unsigned int v) const { return Flags(static_cast<Language>(static_cast<unsigned int>(Language::UserDefined) + v)); }
        };

        static const Propagation_t Propagation; 
        static const Overwrite_t Overwrite;
        static const Type_t Type;
        static const Language_t Language;

        }
    }

    gubg::chai::ModulePtr flags()
    {
        auto ptr = gubg::chai::make_module();


#define EXPOSE_TOP(TYPE)  \
ptr->add_global_const(chaiscript::const_var(wrapper::TYPE), #TYPE); \
ptr->add(chaiscript::user_type<TYPE>(), #TYPE); \
ptr->add(chaiscript::user_type<wrapper::TYPE##_t>(), #TYPE); \
ptr->add(chaiscript::type_conversion<TYPE, Flags>()); \
ptr->add(chaiscript::fun([](const wrapper::TYPE##_t &) { return std::string(#TYPE); }), "to_string")
        EXPOSE_TOP(Propagation);
        EXPOSE_TOP(Overwrite);
        EXPOSE_TOP(Type);
        EXPOSE_TOP(Language);
#undef EXPOSE_TOP

#define EXPOSE(TYPE, NAME) ptr->add(chaiscript::fun([](const wrapper::TYPE##_t & ) { return Flags(TYPE::NAME); }), #NAME)
        EXPOSE(Propagation, Public);
        EXPOSE(Propagation, Private);
        EXPOSE(Overwrite, Always);
        EXPOSE(Overwrite, IfSame);
        EXPOSE(Overwrite, Never);
        EXPOSE(Type, Undefined);
        EXPOSE(Type, Source);
        EXPOSE(Type, Header);
        EXPOSE(Type, Export);
        EXPOSE(Type, Object);
        EXPOSE(Type, ForceInclude);
        EXPOSE(Type, IncludePath);
        EXPOSE(Type, LibraryPath);
        EXPOSE(Type, Library);
        EXPOSE(Type, FrameworkPath);
        EXPOSE(Type, Framework);
        EXPOSE(Type, Dependency);
        EXPOSE(Type, Define);
        EXPOSE(Type, Executable);
        EXPOSE(Language, Undefined);
        EXPOSE(Language, Binary);
        EXPOSE(Language, C);
        EXPOSE(Language, CXX);
        EXPOSE(Language, ASM);
        EXPOSE(Language, Script);
        EXPOSE(Language, Definition);
#undef EXPOSE
        
        ptr->add(chaiscript::fun(&wrapper::Type_t::make_UserDefined), "UserDefined");
        ptr->add(chaiscript::fun(&wrapper::Language_t::make_UserDefined), "UserDefined");


        ptr->add(chaiscript::user_type<Flags>(), "Flags");
        ptr->add(chaiscript::constructor<Flags()>(), "Flags");
        ptr->add(chaiscript::fun([](Flags & f, const Flags & to_set) { f.set(to_set); } ), "set");
        ptr->add(chaiscript::fun(&Flags::to_string), "to_string");
        ptr->add(chaiscript::fun([](const Flags & lhs, const Flags & rhs) { return lhs&rhs; } ), "&");
        ptr->add(chaiscript::fun([](const Flags & lhs, const Flags & rhs) { return lhs||rhs; } ), "|");
        ptr->add(chaiscript::type_conversion<Flags, bool>());
        ptr->add(chaiscript::fun(&Flags::operator==), "==");
        ptr->add(chaiscript::fun(&Flags::operator!=), "!=");

        return ptr;
    }

} } }


#include "cook/chai/module/ToolchainElement.hpp"
#include "cook/chai/module/Recipe.hpp"
#include "cook/chai/module/EnumHelper.hpp"
#include "cook/chai/ToolchainElement.hpp"
#include "gubg/chai/Module.hpp"

namespace cook { namespace chai { namespace module {
    
    CREATE_WRAPPER_TYPE(Part);
    CREATE_WRAPPER_TYPE(ElementType);
    
    using IngredientProcessor = std::function<bool (ToolchainElement e, Recipe r, chaiscript::Boxed_Value vt)>;

    namespace {

        void set_ingredient_processor(ToolchainElement & element, IngredientProcessor process)
        {
            auto process_file = [=](ToolchainElement e, Recipe r, const File & file)
            {
                return process(e, r, chaiscript::const_var(file));
            };
            element.set_file_processing_function(process_file);
            
            auto process_key_value = [=](ToolchainElement e, Recipe r, const KeyValue & kv)
            {
                return process(e, r, chaiscript::const_var(kv));
            };
            element.set_key_value_processing_function(process_key_value);
        }

    }

    gubg::chai::ModulePtr toolchain_element()
    {
        auto ptr = gubg::chai::make_module();

        EXPOSE_TYPE(Part);

        EXPOSE_VALUE(Part, Cli);
        EXPOSE_VALUE(Part, Pre);
        EXPOSE_VALUE(Part, Runtime);
        EXPOSE_VALUE(Part, Deps);
        EXPOSE_VALUE(Part, Response);
        EXPOSE_VALUE(Part, Export);
        EXPOSE_VALUE(Part, Output);
        EXPOSE_VALUE(Part, Input);
        EXPOSE_VALUE(Part, DepFile);
        EXPOSE_VALUE(Part, Option);
        EXPOSE_VALUE(Part, Define);
        EXPOSE_VALUE(Part, IncludePath);
        EXPOSE_VALUE(Part, ForceInclude);
        EXPOSE_VALUE(Part, Library);
        EXPOSE_VALUE(Part, LibraryPath);
        EXPOSE_VALUE(Part, Framework);
        EXPOSE_VALUE(Part, FrameworkPath);
        EXPOSE_VALUE(Part, Resource);

        EXPOSE_TYPE(ElementType);
        EXPOSE_VALUE(ElementType, Compile);
        EXPOSE_VALUE(ElementType, Link);
        EXPOSE_VALUE(ElementType, Archive);

        // the key values
        ptr->add(chaiscript::user_type<KeyValues>(), "KeyValues");
        ptr->add(chaiscript::fun(&KeyValues::clear), "clear");
        ptr->add(chaiscript::fun(&KeyValues::append_1), "append");
        ptr->add(chaiscript::fun(&KeyValues::append_2), "append");

        // the translators
        ptr->add(chaiscript::user_type<Translators>(), "Translators");
        ptr->add(chaiscript::user_type<process::toolchain::Translator>(), "Translator");
        ptr->add(chaiscript::fun(&Translators::operator[]), "[]");
 
        // the toolchain elements
        ptr->add(chaiscript::user_type<ToolchainElement>(), "ToolchainElement");

        ptr->add(chaiscript::fun(&ToolchainElement::key_values), "key_values");
        ptr->add(chaiscript::fun(&ToolchainElement::translators), "translators");
        ptr->add(chaiscript::fun(&ToolchainElement::language), "language");
        ptr->add(chaiscript::fun(&ToolchainElement::target_type), "target_type");
        ptr->add(chaiscript::fun(&ToolchainElement::element_type), "element_type");
        
        ptr->add(chaiscript::fun(set_ingredient_processor), "set_ingredient_processor");

        return ptr;
    }


} } } 

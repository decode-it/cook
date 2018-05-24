#ifndef HEADER_cook_chai_RaiiIngredient_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_RaiiIngredient_hpp_ALREADY_INCLUDED

#include "cook/LanguageTypePair.hpp"
#include "cook/chai/Context.hpp"
#include "cook/chai/File.hpp"
#include "cook/chai/KeyValue.hpp"

namespace cook { namespace chai {

    template <typename SourceType, typename ChaiType>
    struct RaiiIngredientBase
    {
        RaiiIngredientBase(LanguageTypePair & ltp, SourceType & ingredient, const Context * context)
        : ltp_(ltp), ingredient_(ingredient), chai(ltp, ingredient, context)
        {
        }

        ~RaiiIngredientBase()
        {
            // update the flags back into the references
            ltp_ = chai.language_type_pair();
            chai.install_flags();

            // set the reference to the element
            ingredient_ = chai.element();
        }

        private:
        LanguageTypePair & ltp_;
        SourceType & ingredient_;

        public:
        ChaiType chai;
    };

    using RaiiFile = RaiiIngredientBase<ingredient::File, File>;
    using RaiiKeyValue = RaiiIngredientBase<ingredient::KeyValue, KeyValue>;


} }

#endif


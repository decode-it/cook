#ifndef HEADER_cook_ingredient_Base_hpp_ALREADY_INCLUDED
#define HEADER_cook_ingredient_Base_hpp_ALREADY_INCLUDED

#include "cook/Propagation.hpp"
#include "cook/Overwrite.hpp"
#include "cook/Result.hpp"
#include "cook/Content.hpp"
#include "gubg/mss.hpp"

namespace cook {

    namespace model {
        class Recipe;
    }

    namespace ingredient {

        inline bool merge(model::Recipe *& old_value, model::Recipe * new_value)
        {
            if (new_value)
                old_value = new_value;

            return true;
        }

        template <typename KeyType>
        class Base
        {
        public:
            using key_type = KeyType;

            explicit Base(const KeyType & key)
            : key_(key),
            propagation_(Propagation::Private),
            owner_(nullptr),
            overwrite_(Overwrite::Never),
            content_(Content::Generated)
            {
            }

            const KeyType & key() const                     { return key_; }
            model::Recipe * owner() const                   { return owner_; }
            Propagation propagation() const                 { return propagation_; }
            Overwrite overwrite() const                     { return overwrite_; }
            Content content() const                         { return content_; }
            void set_propagation(Propagation propagation)   { propagation_ = propagation; }
            void set_overwrite(Overwrite overwrite)         { overwrite_ = overwrite; }
            void set_owner(model::Recipe * owner)           { owner_ = owner; }
            void set_content(Content content)               { content_ = content; }

        protected:
            bool equal_(const Base<KeyType> & rhs) const
            {
                return key_ == rhs.key_
                && propagation_ == rhs.propagation_;
            }

            template <typename Ingredient>
            static Result merge_(Ingredient & lhs, const Ingredient & rhs)
            {
                MSS_BEGIN(Result);
                MSS(lhs.key() == rhs.key());

                // should we update the overwrite policy ?
                Overwrite ov = lhs.overwrite();
                merge(ov, rhs.overwrite());

                if (false) {}
                else if (lhs.overwrite() > rhs.overwrite())
                    MSG_MSS(true, Warning, "Refining overwrite for " << lhs << " to " << ov);
                else if (rhs.overwrite() > lhs.overwrite())
                    MSG_MSS(true, Warning, "Refining overwrite for " << rhs << " to " << ov);

                switch(ov)
                {
                    case Overwrite::Always:
                        break;

                    case Overwrite::IfSame:
                        MSG_MSS(lhs == rhs, Error, rhs << " != " << lhs << " and overwrite = " << ov);
                        break;

                    default:
                        MSG_MSS(false, Error, "overwrite = " << ov);
                        break;
                }

                lhs.overwrite_ = ov;
                MSS(merge(lhs.propagation_, rhs.propagation()));
                MSS(merge(lhs.owner_, rhs.owner()));

                MSS_END();
            }

        private:
            KeyType key_;
            Propagation propagation_;
            model::Recipe * owner_;
            Overwrite overwrite_;
            Content content_;
        };

    }
}

#endif

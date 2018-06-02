#ifndef HEADER_cook_ingredient_Collection_hpp_ALREADY_INCLUDED
#define HEADER_cook_ingredient_Collection_hpp_ALREADY_INCLUDED

#include "cook/ingredient/Base.hpp"
#include "gubg/Range.hpp"
#include <vector>
#include <algorithm>

namespace cook { namespace ingredient {

    template <typename Ingredient>
    class Collection
    {
        using Container = std::vector<Ingredient>;

    public:
        using key_type = typename Ingredient::key_type;
        using const_iterator = typename Container::const_iterator;
        using iterator = typename Container::iterator;

        void clear() { ingredients_.clear(); }

        std::pair<iterator, bool> insert(const Ingredient & ingredient) { return insert_(ingredient); }
        std::pair<iterator, bool> insert(Ingredient && ingredient)      { return insert_(std::move(ingredient)); }

        const_iterator find(const key_type & key) const { return find_(gubg::make_range(ingredients_), key); }
        iterator find(const key_type & key)             { return find_(gubg::make_range(ingredients_), key); }

        const_iterator begin() const    { return ingredients_.begin(); }
        const_iterator end() const      { return ingredients_.end(); }
        iterator begin()                { return ingredients_.begin(); }
        iterator end()                  { return ingredients_.end(); }

        iterator erase(iterator position)               { return ingredients_.erase(position); }
        std::size_t erase(const key_type & key)
        {
            auto it = std::remove_if(ingredients_.begin(), ingredients_.end(), [&](const auto & el) { return el.key() == key; });
            std::size_t removed = std::distance(it, ingredients_.end());
            ingredients_.erase(it, ingredients_.end());
            return removed;
        }

        std::size_t size() const { return ingredients_.size(); }

    private:
        template <typename It>
        static It find_(gubg::Range<It> range, const key_type & key)
        {
            return std::find_if(range.begin(), range.end(), [&](const auto & el) { return el.key() == key; });
        }

        template <typename AIngredient>
        std::pair<iterator, bool> insert_(AIngredient && ingredient)
        {
            iterator it = find_(gubg::make_range(ingredients_), ingredient.key());

            // already present ?
            if (it != end())
                return std::make_pair(it, false);

            // nope, insert it
            ingredients_.push_back(std::forward<AIngredient>(ingredient));
            it = (ingredients_.end() - 1);
            return std::make_pair(it, true);
        }

        std::vector<Ingredient> ingredients_;
    };


} }

#endif

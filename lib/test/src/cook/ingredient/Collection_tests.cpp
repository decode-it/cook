#include "catch.hpp"
#include "cook/ingredient/Collection.hpp"
#include "cook/ingredient/Base.hpp"

namespace {

struct Ingredient : public cook::ingredient::Base<std::string>
{
    Ingredient(const std::string &key, unsigned int id)
        : cook::ingredient::Base<std::string>(key),
          id_(id)
    {
    }

    bool operator==(const Ingredient & rhs) const { return equal_(rhs) && id_ == rhs.id_; }

    unsigned int id() const { return id_; }

private:
    unsigned int id_ = 0;
};
using Ingredients = cook::ingredient::Collection<Ingredient>;



struct AddScenario
{
    std::vector<Ingredient> initial_ingredients;
    std::optional<Ingredient> ingredient_to_add;

    bool can_add = true;
};

}

TEST_CASE("Ingredient Collection addition test", "[ut][ingredients]")
{
    AddScenario scn;

    SECTION("no ingredients")
    {
        scn.can_add = true;

        SECTION("empty ingredient") { scn.ingredient_to_add = Ingredient("", 0); }
        SECTION("non-empty ingredient") { scn.ingredient_to_add = Ingredient("test", 0); }
    }

    SECTION("single ingredient")
    {
        scn.initial_ingredients.push_back(Ingredient("a", 0));

        SECTION("success")
        {
            scn.can_add = true;
            scn.ingredient_to_add = Ingredient("b", 0);
        }

        SECTION("failure")
        {
            scn.can_add = false;
            SECTION("identical") { scn.ingredient_to_add = Ingredient("a", 0); }
            SECTION("same key")  { scn.ingredient_to_add = Ingredient("a", 10); }
        }
    }

    SECTION("multiple ingredients")
    {
        scn.initial_ingredients.push_back(Ingredient("a", 0));
        scn.initial_ingredients.push_back(Ingredient("b", 2));
        scn.initial_ingredients.push_back(Ingredient("c", 3));


        SECTION("success")
        {
            scn.can_add = true;
            scn.ingredient_to_add = Ingredient("d", 0);
        }

        SECTION("failure")
        {
            scn.can_add = false;

            SECTION("same key")
            {
                SECTION("key: a") { scn.ingredient_to_add = Ingredient("a", 10);  }
                SECTION("key: b") { scn.ingredient_to_add = Ingredient("b", 10);  }
                SECTION("key: c") { scn.ingredient_to_add = Ingredient("c", 10);  }
            }

            SECTION("identical")
            {
                SECTION("key: a") { scn.ingredient_to_add = scn.initial_ingredients[0]; }
                SECTION("key: b") { scn.ingredient_to_add = scn.initial_ingredients[1]; }
                SECTION("key: c") { scn.ingredient_to_add = scn.initial_ingredients[2]; }
            }
        }
    }


    // process the scenario
    {
        Ingredients ingredients;

        // add the initial ingredients
        for(const auto & ingredient : scn.initial_ingredients)
        {
            auto p = ingredients.insert(ingredient);
            // we should be able to add them all
            REQUIRE(p.second);
        }

        // check them again, same order
        REQUIRE(std::equal(ingredients.begin(), ingredients.end(), scn.initial_ingredients.begin(), scn.initial_ingredients.end()));

        REQUIRE(scn.ingredient_to_add);
        const Ingredient & ingredient = *scn.ingredient_to_add;

        // check for addition
        {
            auto it = ingredients.find(ingredient.key());
            bool valid_it = it != ingredients.end();
            REQUIRE(valid_it != scn.can_add);

            if (valid_it)
                REQUIRE(it->key() == ingredient.key());
        }

        // add
        {
            auto p = ingredients.insert(ingredient);

            // always a valid iterator
            REQUIRE(p.first != ingredients.end());
            REQUIRE(p.first->key() == ingredient.key());

            REQUIRE(p.second == scn.can_add);
        }

        // first n should remain the same
        {
            auto first = ingredients.begin();
            auto last = ingredients.begin();
            std::advance(last, scn.initial_ingredients.size());

            REQUIRE(std::equal(first, last, scn.initial_ingredients.begin(), scn.initial_ingredients.end()));

            bool at_last = last == ingredients.end();
            REQUIRE(at_last != scn.can_add);

            // still element after last ?
            if (!at_last)
            {
                // last should be pointing to the new
                REQUIRE(*last == ingredient);

                // and this is the last element
                REQUIRE(std::distance(last, ingredients.end()) == 1);
            }
        }

        // size check
        {
            REQUIRE(ingredients.size() == (scn.initial_ingredients.size() + scn.can_add));
        }

    }
}


namespace {

struct EraseScenario
{
    std::string key;
    bool removed = false;
};

}

TEST_CASE("Ingredient Collection erase test", "[ut][ingredient][collection]")
{
    Ingredients ingredients;

    for(char ch = 'a'; ch <= 'd'; ++ch)
        REQUIRE(ingredients.insert(Ingredient(std::string(1, ch), 0)).second);

    EraseScenario scn;

    SECTION("unknown key")
    {
        scn.key = "e";
        scn.removed = false;
    }
        SECTION("success")
        {
            scn.removed = true;

            SECTION("a") { scn.key = "a"; }
            SECTION("b") { scn.key = "b"; }
            SECTION("c") { scn.key = "c"; }
            SECTION("d") { scn.key = "d"; }
        }

    // erase by key
    {
        Ingredients p1 = ingredients;
        std::size_t removed = p1.erase(scn.key);

        REQUIRE(scn.removed == removed);

        // make sure that it is actually removed
        REQUIRE(removed + p1.size() == ingredients.size());
        REQUIRE(p1.find(scn.key) == p1.end());

        // try to erase again should not be possible
        REQUIRE(p1.erase(scn.key) == 0);
    }

    // erase by iterator
    {
        Ingredients p1 = ingredients;
        auto old_it = p1.find(scn.key);

        bool found = (old_it  != p1.end());
        REQUIRE( found == scn.removed );

        if (found)
        {

            // remove it
            const std::size_t before_count = std::distance(p1.begin(), old_it);
            auto new_it = p1.erase(old_it);
            const std::size_t after_count = std::distance(new_it, p1.end());


            // check sizes
            const std::size_t count = before_count + after_count;
            REQUIRE(count == p1.size());
            REQUIRE(p1.size() + scn.removed == ingredients.size());

            // check order the same
            REQUIRE(std::equal(ingredients.begin(), ingredients.begin() + before_count, p1.begin(), p1.begin() + before_count));
            REQUIRE(std::equal(ingredients.end() - after_count, ingredients.end(), p1.end() - after_count, p1.end()));

            // make sure that it is actually removed
            REQUIRE(p1.find(scn.key) == p1.end());

            // try to erase again should not be possible
            REQUIRE(p1.erase(scn.key) == 0);
            /// }
        }
    }
}


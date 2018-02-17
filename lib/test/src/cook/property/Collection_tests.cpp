#include "catch.hpp"
#include "cook/property/Collection.hpp"
#include "cook/property/Interface.hpp"

namespace {

struct Property : public cook::property::Interface<std::string>
{
    Property(const std::string &key, unsigned int id)
        : cook::property::Interface<std::string>(key),
          id_(id)
    {
    }

    bool operator==(const Property & rhs) const { return equal_(rhs) && id_ == rhs.id_; }

    unsigned int id() const { return id_; }

private:
    unsigned int id_ = 0;
};
using Properties = cook::property::Collection<Property>;



struct AddScenario
{
    std::vector<Property> initial_properties;
    std::optional<Property> property_to_add;

    bool can_add = true;
};

}

TEST_CASE("Property Collection addition test", "[ut][propertySet]")
{
    AddScenario scn;

    SECTION("no properties")
    {
        scn.can_add = true;

        SECTION("empty property") { scn.property_to_add = Property("", 0); }
        SECTION("non-empty property") { scn.property_to_add = Property("test", 0); }
    }

    SECTION("single property")
    {
        scn.initial_properties.push_back(Property("a", 0));

        SECTION("success")
        {
            scn.can_add = true;
            scn.property_to_add = Property("b", 0);
        }

        SECTION("failure")
        {
            scn.can_add = false;
            SECTION("identical") { scn.property_to_add = Property("a", 0); }
            SECTION("same key")  { scn.property_to_add = Property("a", 10); }
        }
    }

    SECTION("multiple properties")
    {
        scn.initial_properties.push_back(Property("a", 0));
        scn.initial_properties.push_back(Property("b", 2));
        scn.initial_properties.push_back(Property("c", 3));


        SECTION("success")
        {
            scn.can_add = true;
            scn.property_to_add = Property("d", 0);
        }

        SECTION("failure")
        {
            scn.can_add = false;

            SECTION("same key")
            {
                SECTION("key: a") { scn.property_to_add = Property("a", 10);  }
                SECTION("key: b") { scn.property_to_add = Property("b", 10);  }
                SECTION("key: c") { scn.property_to_add = Property("c", 10);  }
            }

            SECTION("identical")
            {
                SECTION("key: a") { scn.property_to_add = scn.initial_properties[0]; }
                SECTION("key: b") { scn.property_to_add = scn.initial_properties[1]; }
                SECTION("key: c") { scn.property_to_add = scn.initial_properties[2]; }
            }
        }
    }


    // process the scenario
    {
        Properties properties;

        // add the initial properties
        for(const auto & property : scn.initial_properties)
        {
            auto p = properties.insert(property);
            // we should be able to add them all
            REQUIRE(p.second);
        }

        // check them again, same order
        REQUIRE(std::equal(properties.begin(), properties.end(), scn.initial_properties.begin(), scn.initial_properties.end()));

        REQUIRE(scn.property_to_add);
        const Property & property = *scn.property_to_add;

        // check for addition
        {
            auto it = properties.find(property.key());
            bool valid_it = it != properties.end();
            REQUIRE(valid_it != scn.can_add);

            if (valid_it)
                REQUIRE(it->key() == property.key());
        }

        // add
        {
            auto p = properties.insert(property);

            // always a valid iterator
            REQUIRE(p.first != properties.end());
            REQUIRE(p.first->key() == property.key());

            REQUIRE(p.second == scn.can_add);
        }

        // first n should remain the same
        {
            auto first = properties.begin();
            auto last = properties.begin();
            std::advance(last, scn.initial_properties.size());

            REQUIRE(std::equal(first, last, scn.initial_properties.begin(), scn.initial_properties.end()));

            bool at_last = last == properties.end();
            REQUIRE(at_last != scn.can_add);

            // still element after last ?
            if (!at_last)
            {
                // last should be pointing to the new
                REQUIRE(*last == property);

                // and this is the last element
                REQUIRE(std::distance(last, properties.end()) == 1);
            }
        }

        // size check
        {
            REQUIRE(properties.size() == (scn.initial_properties.size() + scn.can_add));
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

TEST_CASE("Property Collection erase test", "[ut][property][collection]")
{
    Properties properties;

    for(char ch = 'a'; ch <= 'd'; ++ch)
        REQUIRE(properties.insert(Property(std::string(1, ch), 0)).second);

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
        Properties p1 = properties;
        std::size_t removed = p1.erase(scn.key);

        REQUIRE(scn.removed == removed);

        // make sure that it is actually removed
        REQUIRE(removed + p1.size() == properties.size());
        REQUIRE(p1.find(scn.key) == p1.end());

        // try to erase again should not be possible
        REQUIRE(p1.erase(scn.key) == 0);
    }

    // erase by iterator
    {
        Properties p1 = properties;
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
            REQUIRE(p1.size() + scn.removed == properties.size());

            // check order the same
            REQUIRE(std::equal(properties.begin(), properties.begin() + before_count, p1.begin(), p1.begin() + before_count));
            REQUIRE(std::equal(properties.end() - after_count, properties.end(), p1.end() - after_count, p1.end()));

            // make sure that it is actually removed
            REQUIRE(p1.find(scn.key) == p1.end());

            // try to erase again should not be possible
            REQUIRE(p1.erase(scn.key) == 0);
            /// }
        }
    }
}


#include "catch.hpp"
#include "cook/model/PropertySet.hpp"
#include "cook/model/Property.hpp"

namespace {

struct Property : public cook::model::Property<std::string>
{
    Property(const std::string &key, unsigned int id)
        : cook::model::Property<std::string>(key, nullptr),
          id_(id)
    {
    }

    bool operator==(const Property & rhs) const { return equal_(rhs) && id_ == rhs.id_; }

    unsigned int id() const { return id_; }

private:
    unsigned int id_;
};
using PropertySet = cook::model::PropertySet<Property>;



struct Scenario
{
    std::vector<Property> initial_properties;
    std::optional<Property> property_to_add;

    bool can_add;
};


}

TEST_CASE("PropertySet", "[ut][propertySet]")
{
    Scenario scn;

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
        PropertySet properties(cook::Language::UserDefined, cook::Type::UserDefined);

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

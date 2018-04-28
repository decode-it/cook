#include "catch.hpp"
#include "cook/model/Uri.hpp"

using Uri = cook::model::Uri;

namespace  {

struct ConstructionScenario
{
    std::string input;
    bool is_absolute = false;
    unsigned int path_size = 0;
};

}

TEST_CASE("Uri construction tests", "[ut][uri]")
{
    ConstructionScenario scn;
    scn.input = "";


    SECTION("success")
    {

        SECTION("relative")
        {
            scn.is_absolute = false;
            SECTION("empty")                { scn.path_size = 0; }
            SECTION("single")               { scn.input = "name"; scn.path_size = 1; }
            SECTION("multiple")             { scn.input = "a/b/c/name"; scn.path_size = 4; }
            SECTION("single, trailing")     { scn.input = "name/"; scn.path_size = 1; }
            SECTION("multiple, trailing")   { scn.input = "a/b/c/name/"; scn.path_size = 4; }
        }
        SECTION("absolute")
        {
            scn.is_absolute = true;
            SECTION("/")                    { scn.input = "/"; scn.path_size = 0; }
            SECTION("single")               { scn.input = "/name"; scn.path_size = 1; }
            SECTION("multiple")             { scn.input = "/a/b/c/name"; scn.path_size = 4; }
            SECTION("single, trailing")     { scn.input = "/name/"; scn.path_size = 1; }
            SECTION("multiple, trailing")   { scn.input = "/a/b/c/name/"; scn.path_size = 4; }
        }
    }

    cook::model::Uri uri(scn.input);


    REQUIRE(uri.absolute() == scn.is_absolute);

    auto rng = uri.path();
    REQUIRE(std::distance(rng.begin(), rng.end()) == scn.path_size);
    REQUIRE(cook::model::Uri(uri.string()) == uri);
}

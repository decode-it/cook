#include "catch.hpp"
#include "cook/model/Uri.hpp"

using Uri = cook::model::Uri;

namespace  {

enum Type { Recipe, Book };

struct ConstructionScenario
{
    std::string input;
    Type type;
    bool can_construct = false;
    bool is_absolute = false;
    unsigned int path_size = 0;
};

std::pair<Uri, bool> make_uri(const std::string & input, Type type)
{
    switch(type)
    {
        case Recipe:
            return Uri::recipe_uri(input);
        case Book:
            return Uri::book_uri(input);
        default:
            REQUIRE(false);
            return std::make_pair(Uri(), false);
    }
}

}

TEST_CASE("Uri construction tests", "[ut][uri]")
{
    ConstructionScenario scn;
    scn.input = "";


    SECTION("success")
    {
        scn.can_construct = true;

        SECTION("as recipe")
        {
            scn.type = Recipe;
            SECTION("relative")
            {
                scn.is_absolute = false;
                SECTION("single")               { scn.input = "name"; scn.path_size = 0; }
                SECTION("multiple")             { scn.input = "a/b/c/name"; scn.path_size = 3; }
            }
            SECTION("absolute")
            {
                scn.is_absolute = true;
                SECTION("single")               { scn.input = "/name"; scn.path_size = 0; }
                SECTION("multiple")             { scn.input = "/a/b/c/name"; scn.path_size = 3; }
            }
        }
        SECTION("as book")
        {
            scn.type = Book;

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
    }

    SECTION("failure")
    {
        scn.can_construct = false;

        SECTION("as recipe")
        {
            scn.type = Recipe;
            SECTION("empty") { }
            SECTION("single, trailing slash")   { scn.input = "a/"; }
            SECTION("multiple, trailing slash") { scn.input = "/a/b/c/a/"; }
            SECTION("double slash")
            {
                SECTION("begin")                { scn.input = "//a/b/b/c"; }
                SECTION("middle")               { scn.input = "a/b//b/c"; }
                SECTION("end")                  { scn.input = "a/b/b/c//"; }
            }
        }

        SECTION("as book")
        {
            scn.type = Book;
            SECTION("double slash")
            {
                SECTION("begin")                { scn.input = "//a/b/b/c"; }
                SECTION("middle")               { scn.input = "a/b//b/c"; }
                SECTION("end")                  { scn.input = "a/b/b/c//"; }
            }
        }
    }

    std::pair<Uri, bool> p = make_uri(scn.input, scn.type);
    REQUIRE(p.second == scn.can_construct);

    if (p.second)
    {
        const Uri & uri = p.first;

        REQUIRE(uri.absolute() == scn.is_absolute);

        auto rng = uri.path();
        REQUIRE(std::distance(rng.begin(), rng.end()) == scn.path_size);

        {
            std::string hr = uri.string();
            REQUIRE(hr.size() >= scn.input.size());
            REQUIRE(hr.substr(0, scn.input.size()) == scn.input);

            if (hr.size() != scn.input.size())
            {
                REQUIRE(hr.size() == scn.input.size() + 1);
                REQUIRE(hr[scn.input.size()] == Uri::separator);
            }
        }
//        REQUIRE(== scn.input);
    }
}

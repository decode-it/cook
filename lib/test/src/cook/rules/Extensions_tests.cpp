#include "catch.hpp"
#include "cook/rules/Extensions.hpp"
using namespace cook;

TEST_CASE("cook::rules::Extensions<CXX> tests", "[ut][rules][Extensions]")
{
    struct Scn
    {
        std::string extension;
        Type type = Type::Undefined;
    };
    Scn scn;

    struct Should
    {
        bool type_is_known = false;
        Type type = Type::Undefined;
    };
    Should should;

    SECTION("positive")
    {
        should.type_is_known = true;

        SECTION("type not known upfront")
        {
            SECTION("source")
            {
                should.type = Type::Source;
                SECTION(".cpp") { scn.extension = ".cpp"; }
                SECTION(".cxx") { scn.extension = ".cxx"; }
                SECTION(".CPP") { scn.extension = ".CPP"; }
                SECTION(".CXX") { scn.extension = ".CXX"; }
            }
            SECTION("header")
            {
                should.type = Type::Header;
                SECTION(".hpp") { scn.extension = ".hpp"; }
                SECTION(".hxx") { scn.extension = ".hxx"; }
                SECTION(".HPP") { scn.extension = ".HPP"; }
                SECTION(".HXX") { scn.extension = ".HXX"; }
            }
        }
        SECTION("type already known upfront")
        {
            scn.type = Type::Source;
            should.type = scn.type;
            SECTION(".cpp") { scn.extension = ".cpp"; }
            SECTION(".hpp") { scn.extension = ".hpp"; }
            SECTION(".unknown") { scn.extension = ".unkown"; }
        }

        REQUIRE(should.type != Type::Undefined);
    }
    SECTION("negative")
    {
        should.type_is_known = false;

        SECTION("default") { }
        SECTION(".unknown") { scn.extension = ".unkown"; }
    }

    Type type;
    rules::Extensions exts{Language::CXX};
    const auto type_is_known = exts.get_type(type, scn.extension, scn.type);
    REQUIRE(type_is_known == should.type_is_known);
    if (type_is_known)
        REQUIRE(type == should.type);
}


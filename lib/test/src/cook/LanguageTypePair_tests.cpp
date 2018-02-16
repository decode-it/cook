#include "catch.hpp"
#include "cook/LanguageTypePair.hpp"
#include "gubg/macro/capture.hpp"
#include <iostream>
using namespace cook;

TEST_CASE("cook::LanguageTypePair tests", "[ut][LanguageTypePair]")
{
    LanguageTypePair a, b;

    struct Should
    {
        bool le = false;
        bool eq = true;
    };
    Should should;

    SECTION("language <")
    {
        a.language = Language::C;
        b.language = Language::CXX;
        b.type = Type::Header;
        SECTION("type <")  { a.type = Type::Source; }
        SECTION("type ==") { a.type = Type::Header; }
        SECTION("type >")  { a.type = Type::Object; }
        should.le = true;
        should.eq = false;
    }
    SECTION("language ==")
    {
        a.language = Language::CXX;
        b.language = Language::CXX;
        b.type = Type::Header;
        SECTION("type <")
        {
            a.type = Type::Source;
            should.le = true;
            should.eq = false;
        }
        SECTION("type ==")
        {
            a.type = Type::Header;
            should.le = false;
            should.eq = true;
        }
        SECTION("type >")
        {
            a.type = Type::Object;
            should.le = false;
            should.eq = false;
        }
    }
    SECTION("language >")
    {
        a.language = Language::ASM;
        b.language = Language::CXX;
        b.type = Type::Header;
        SECTION("type <")  { a.type = Type::Source; }
        SECTION("type ==") { a.type = Type::Header; }
        SECTION("type >")  { a.type = Type::Object; }
        should.le = false;
        should.eq = false;
    }

    std::cout << C(a)C(b) << std::endl;
    REQUIRE((a < b) == should.le);
    REQUIRE((a == b) == should.eq);
}

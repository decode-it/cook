#include "catch.hpp"
#include "cook/model/Globber.hpp"

#define BASE_DIR "generated/"

namespace  {



void create_files()
{
    std::filesystem::create_directories(BASE_DIR "subdir");
    std::ofstream(BASE_DIR "file1.cpp");
    std::ofstream(BASE_DIR "file1.hpp");
    std::ofstream(BASE_DIR "other.txt");
    std::ofstream(BASE_DIR "subdir/file2.cpp");
    std::ofstream(BASE_DIR "subdir/file2.hpp");
}

void clear_files()
{
    std::filesystem::remove_all(BASE_DIR);
}

struct Scenario
{
    std::set<std::string>               expected_files;
    std::optional<cook::Language>       language;
    std::optional<cook::Type>           type;
    std::optional<cook::Overwrite>      overwrite;
    std::optional<cook::Propagation>    propagation;
};

template <typename T>
bool check_if_equal(std::optional<T> t, T value, T unset_value)
{
    MSS_BEGIN(bool);

    bool optional_set = t.has_value();
    bool value_set = (value != unset_value);

    MSS(optional_set == value_set);
    if (t)
        MSS(*t == value);
    MSS_END();
}

}



TEST_CASE("globber tests", "[ut][globber]")
{
    create_files();

    cook::model::Globber globber;

    Scenario scn;


    SECTION("dir/pattern tests")
    {
        SECTION("dir as base dir")
        {
            globber.dir = BASE_DIR;



            SECTION("all files")
            {
                globber.pattern = "**";
                scn.expected_files = {"subdir", "file1.cpp", "file1.hpp", "other.txt", "subdir/file2.cpp", "subdir/file2.hpp"};
            }

            SECTION("non-recursive files")
            {
                globber.pattern = "*";
                scn.expected_files = {"subdir", "file1.cpp", "file1.hpp", "other.txt"};
            }

            SECTION("extension files")
            {
                globber.pattern = "**.[hc]pp";
                scn.expected_files = {"file1.cpp", "file1.hpp", "subdir/file2.cpp", "subdir/file2.hpp"};
            }

            SECTION("non-recursive, extension files")
            {
                globber.pattern = "*.[hc]pp";
                scn.expected_files = {"file1.cpp", "file1.hpp"};
            }

            SECTION("subdir files")
            {
                globber.pattern = "subdir/*.[hc]pp";
                scn.expected_files = {"subdir/file2.cpp", "subdir/file2.hpp"};
            }

            SECTION("non-matching pattern")
            {
                globber.pattern = "*.c";
            }
        }
        SECTION("subdir")
        {
            globber.dir = BASE_DIR "subdir";
            globber.pattern = "*";
            scn.expected_files = {"file2.cpp", "file2.hpp"};
        }

        SECTION("non-existing dir")
        {
            globber.dir = BASE_DIR "/tralalal";
        }
    }

    SECTION("properties")
    {
        globber.dir = BASE_DIR;
        globber.pattern = "**.[hc]pp";
        scn.expected_files = {"file1.cpp", "file1.hpp", "subdir/file2.cpp", "subdir/file2.hpp"};

        SECTION("specifying language")      { scn.language = cook::Language::CXX; }
        SECTION("specifying type")          { scn.type = cook::Type::Source; }
        SECTION("specifying overwrite")     { scn.overwrite = cook::Overwrite::Always; }
        SECTION("specifying propagation")   { scn.propagation = cook::Propagation::Public; }
    }


    {
        {
            if (scn.language) globber.language = *scn.language;
            if (scn.type) globber.type= *scn.type;
            if (scn.overwrite) globber.overwrite = *scn.overwrite;
            if (scn.propagation) globber.propagation = *scn.propagation;
        }

        const static cook::Language my_own_language = cook::Language::UserDefined;

        auto resolver = [&](cook::model::property::PropertiesKey & key, cook::model::property::File & f)
        {
            // overwrite the key if not set
            if (key.language == cook::Language::Undefined)
            {
                REQUIRE(!scn.language);
                key.language = my_own_language;
            }
            else
            {
                REQUIRE(scn.language);
                REQUIRE(*scn.language == key.language);
            }

            REQUIRE(check_if_equal(scn.type, key.type, cook::Type::Undefined));
        };

        std::set<std::string> files;
        auto adder = [&](const cook::model::property::PropertiesKey & key, const cook::model::property::File & f)
        {
            // assert correct propagation
            if (key.language == my_own_language)
            {
                REQUIRE(!scn.language);
            }
            else
            {
                REQUIRE(scn.language);
                REQUIRE(*scn.language == key.language);
            }
            files.insert(f.rel());

            REQUIRE(check_if_equal(scn.overwrite, f.overwrite(), cook::Overwrite::Never));
            REQUIRE(check_if_equal(scn.propagation, f.propagation(), cook::Propagation::Private));
        };

        REQUIRE(globber.process(std::filesystem::current_path(), resolver, adder));
        REQUIRE(files == scn.expected_files);
    }

    clear_files();

}

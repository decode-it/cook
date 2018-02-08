#include "catch.hpp"
#include "cook/Result.hpp"

namespace {

cook::Result make_result(cook::result::Type t)
{
    return cook::Result ( t, cook::result::Unknown, 0 );
}

template <typename T> T mss_result_to_T(cook::result::Type t)
{
    MSS_BEGIN(T);

    MSS(make_result(t));

    MSS_END();
}

template <typename T>
cook::Result mss_T_to_result(T value)
{
    MSS_BEGIN(cook::Result);
    MSS(value);
    MSS_END();
}

enum ReturnCode
{
    OK = 0,
    Error = 1,
};

template <typename T> void test_result_to_T()
{
    REQUIRE(mss_result_to_T<T>(cook::result::Success) == gubg::mss::detail::Traits<T>::Ok());
    REQUIRE(mss_result_to_T<T>(cook::result::Warning) == gubg::mss::detail::Traits<T>::Ok());
    REQUIRE(mss_result_to_T<T>(cook::result::Success) != gubg::mss::detail::Traits<T>::Error());
}

}

TEST_CASE("mss and error code test", "[ut][mss][error_code]")
{
    SECTION("result to bool")       { test_result_to_T<bool>(); }
    SECTION("result to int")        { test_result_to_T<int>(); }
    SECTION("result to ReturnCode") { test_result_to_T<ReturnCode>(); }

    SECTION("T to result")
    {
        cook::result::Type type = cook::result::Success;
        cook::result::Category category = cook::result::None;
        int value = 0;
        bool conversion = true;

        cook::Result r;

        SECTION("success")
        {
            SECTION("bool")   { r = mss_T_to_result(true); }
            SECTION("int")    { r = mss_T_to_result(0); }
            SECTION("RC")     { r = mss_T_to_result(OK); }
        }

        SECTION("failure")
        {
            conversion = false;
            type = cook::result::Error;
            category = cook::result::Unknown;

            SECTION("bool")     { r = mss_T_to_result(false);   value = static_cast<int>(false); }
            SECTION("int")      { r = mss_T_to_result(5);       value = static_cast<int>(5);     }
            SECTION("RC")       { r = mss_T_to_result(Error);   value = static_cast<int>(Error); }
        }

        REQUIRE(r == conversion);
        REQUIRE(r.type() == type);
        if (!r)
            REQUIRE(r.value<int>() == value);
    }
}
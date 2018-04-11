#include "catch.hpp"
#include "cook/Log.hpp"

namespace {

    template <typename T> T mss_result_to_T(cook::Message::Type t)
    {
        MSS_BEGIN(T);
        cook::Result res;
        res << cook::Message(t);
        MSS(res);
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
        REQUIRE(mss_result_to_T<T>(cook::Message::Type::Warning) == gubg::mss::detail::Traits<T>::Ok());
        REQUIRE(mss_result_to_T<T>(cook::Message::Type::Error) == gubg::mss::detail::Traits<T>::Error());
        REQUIRE(mss_result_to_T<T>(cook::Message::Type::InternalError) == gubg::mss::detail::Traits<T>::Error());
    }

    struct Scenario
    {
        std::list<cook::Message> messages;
        bool succesful = true;
        unsigned int expected_value = 1;
        unsigned int count = 0;

        const static unsigned int WARNING_MULTIPLIER = 3;
        const static unsigned int ERROR_MULTIPLIER = 5;


        void add(cook::Message::Type type, unsigned int & calculated_value)
        {
            switch(type)
            {
                case cook::Message::Type::Warning:
                    expected_value *= WARNING_MULTIPLIER;
                    messages.push_back(cook::Message(type, gubg::stream([&](auto & os) { calculated_value *= WARNING_MULTIPLIER; } )));
                    break;

                case cook::Message::Type::Error:
                    expected_value *= ERROR_MULTIPLIER;
                    messages.push_back(cook::Message(type, gubg::stream([&](auto & os) { calculated_value *= ERROR_MULTIPLIER; } )));
                    break;
            }
        }
    };

}

TEST_CASE("Result functionality", "[ut][result]")
{
    // histogram to keep track
    unsigned int multiply_result = 1;
    unsigned int message_count = 0;

    Scenario scn;

    SECTION("positive")
    {
        scn.succesful = true;

        SECTION("no message") {}
        SECTION("warning")
        {
            scn.add(cook::Message::Type::Warning, multiply_result);
            scn.count = 1;
        }
        SECTION("combination")
        {
            scn.add(cook::Message::Type::Warning, multiply_result);
            scn.add(cook::Message::Type::Warning, multiply_result);
            scn.count = 2;
        }
    }

    SECTION("failure")
    {
        scn.succesful = false;

        SECTION("single failure")
        {
            scn.add(cook::Message::Type::Error, multiply_result);
            scn.count = 1;
        }

        SECTION("multiple failures")
        {
            scn.add(cook::Message::Type::Error, multiply_result);
            scn.add(cook::Message::Type::Error, multiply_result);
            scn.add(cook::Message::Type::Error, multiply_result);
            scn.count = 3;
        }

        SECTION("warning + failure")
        {
            scn.add(cook::Message::Type::Warning, multiply_result);
            scn.add(cook::Message::Type::Error, multiply_result);
            scn.count = 2;
        }
        SECTION("combination with failure")
        {
            scn.add(cook::Message::Type::Error, multiply_result);
            scn.add(cook::Message::Type::Warning, multiply_result);
            scn.add(cook::Message::Type::Error, multiply_result);
            scn.count = 3;
        }
    }

    {
        cook::Result res;
        for(const auto & p : scn.messages)
            res = res << p;

        REQUIRE(res == scn.succesful);

        // visit all the messages
        {
            unsigned int counted = 0;
            res.each_message([&](const cook::Message & msg)
                    {
                    ++counted;
                    if (!msg.msg_.empty())
                        std::cout << msg.msg_ << std::endl;
                    });
            REQUIRE(counted == scn.count);
        }
    }
}

TEST_CASE("mss and error code test", "[ut][mss][result]")
{
    SECTION("result to bool")       { test_result_to_T<bool>(); }
    SECTION("result to int")        { test_result_to_T<int>(); }
    SECTION("result to ReturnCode") { test_result_to_T<ReturnCode>(); }

    SECTION("T to result")
    {
        bool conversion = true;

        cook::Result r;

        SECTION("success")
        {
            SECTION("bool")   { r = mss_T_to_result(true);  }
            SECTION("int")    { r = mss_T_to_result(0);     }
            SECTION("RC")     { r = mss_T_to_result(OK);    }
        }

        SECTION("failure")
        {
            conversion = false;

            SECTION("bool")       { r = mss_T_to_result(false); }
            SECTION("int")      { r = mss_T_to_result(5);     }
            SECTION("RC")       { r = mss_T_to_result(Error); }
        }

        REQUIRE(r == conversion);
    }
}

namespace {

struct AggregationScenario
{
    std::vector<cook::Message::Type> types;
    std::optional<unsigned int> first_error_position;
};

cook::Result create(cook::Message::Type type)
{
    cook::Result res;
    res << cook::Message(type);
    return res;
}

cook::Result test(const AggregationScenario & scn)
{
    MSS_BEGIN(cook::Result);

    for(unsigned int i = 0; i < scn.types.size(); ++i)
        MSS(create(scn.types[i]), REQUIRE(scn.first_error_position); REQUIRE(*scn.first_error_position == i));

    MSS_END();
}

}

TEST_CASE("aggregation test", "[ut][mss][result]")
{
    AggregationScenario scn;

    SECTION("empty") {}

    SECTION("only positive message")
    {
        SECTION("single warning")   { scn.types = {cook::Message::Type::Warning}; }
        SECTION("combination")      { scn.types = {cook::Message::Type::Warning, cook::Message::Warning }; }
    }

    SECTION("failure")
    {
        SECTION("at first")
        {
            scn.first_error_position = 0;
            SECTION("error")            { scn.types = {cook::Message::Type::Error}; }
            SECTION("internal_error")   { scn.types = {cook::Message::Type::InternalError}; }
        }
    }

    cook::Result res = test(scn);
    {
        const bool is_valid = res;
        const bool expected_valid = !scn.first_error_position;
        REQUIRE(is_valid == expected_valid);
    }

    // count all the messages
    unsigned int cnt = 0;
    res.each_message([&cnt](const cook::Message & ) { ++cnt; });

    if (res)
        REQUIRE(cnt == scn.types.size());
    else
        REQUIRE(cnt == (*scn.first_error_position + 1));
}

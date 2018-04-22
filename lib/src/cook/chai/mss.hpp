#ifndef HEADER_cook_chaiss_mss_hpp_ALREADY_INCLUDED
#define HEADER_cook_chaiss_mss_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "cook/Logger.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace chai {

cook::Logger * log(const Result & rc);
void set_logger(cook::Logger * logger);

} }

#define CHAI_MSS_BEGIN() MSS_BEGIN(::cook::Result)
#define CHAI_MSS_END() ::cook::chai::log(MSS_RC)

//MSS
#if defined(CHAI_MSS) || defined(CHAI_MSS_1) || defined(CHAI_MSS_2) || defined(CHAI_MSS_3)
#error CHAI_MSS macros already defined
#endif
#define CHAI_MSS_1(expr) do { \
    gubg::mss::aggregate(MSS_RC, (expr)); \
    if (!gubg::mss::is_ok(MSS_RC)) \
    { \
        S("CHAI_MSS"); L("Error: " #expr << " failed in \"" << __FILE__ << ":" << __LINE__ << "\""); \
        log(MSS_RC); \
    } \
} while (false)
#define CHAI_MSS_2(expr,action) do { \
    gubg::mss::aggregate(MSS_RC, (expr)); \
    if (!gubg::mss::is_ok(MSS_RC)) \
    { \
        S("CHAI_MSS"); L("Error: " #expr << " failed in \"" << __FILE__ << ":" << __LINE__ << "\""); \
        action; \
        ::cook::chai::log(MSS_RC); \
    } \
} while (false)
#define CHAI_MSS_3(expr,action,aggregator) do { \
    aggregator(MSS_RC, (expr)); \
    if (!gubg::mss::is_ok(MSS_RC)) \
    { \
        S("CHAI_MSS"); L("Error: " #expr << " failed in \"" << __FILE__ << ":" << __LINE__ << "\""); \
        action; \
        log(MSS_RC); \
    } \
} while (false)
#define CHAI_MSS(...) GUBG_GET_ARG_4((__VA_ARGS__, CHAI_MSS_3,CHAI_MSS_2,CHAI_MSS_1))(__VA_ARGS__)

#define CHAI_MSS_MSG(COND, TYPE, STR) CHAI_MSS(COND, {} , ::cook::mss::msg(MESSAGE(TYPE, STR)))

#endif

#include <zysoft/zysoft/memory/auto_buffer.h>
#include "../doctest/doctest.h"

#include "test_define.h"
#include "log.h"

#define TEST_CASE_NAME "auto buffer test"

#define member_v(a) (decltype(a))

static
void Test1()
{
    {
        zysoft::auto_buffer<std::int8_t, 10> ab1{9};
        CHECK(!ab1.empty());
        CHECK(ab1.size() == 9);
        CHECK(ab1.internal_size() == 10);

        zysoft::auto_buffer<std::int8_t, 100> ab2{0};
        CHECK(ab2.empty());
        CHECK(ab2.size() == 0);
        CHECK(ab2.internal_size() == 100);
    }

    {
        zysoft::auto_buffer<std::int8_t, 10> ab1{100};
        int value = 77;
        std::fill(ab1.begin(), ab1.end(), value);
        CHECK(ab1.size() == 100);
        for (const auto& v : ab1) {
            CHECK(v == value);
        }

        ab1.resize(8);
        CHECK(ab1.size() == 8);
        for (const auto& v : ab1) {
            CHECK(v == value);
        }

        zysoft::auto_buffer<std::int8_t, 10> ab{1};
        CHECK(ab.resize(10));
    }
    {
        // 以定义会导致编译期错误
        // zysoft::auto_buffer<std::string, 1> ab1{0};
        // zysoft::auto_buffer<int, 0> ab2{0};
    }
}

#if 1
TEST_CASE(TEST_CASE_NAME)
{
    LogInfo << TEST_CASE_NAME;
    try {
        Test1();
    } catch (const std::exception& e) {
        LogWarn << e.what();
        CHECK(false);
    }
}
#endif



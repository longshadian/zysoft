#include <zysoft/zysoft/random.h>

#include "../doctest/doctest.h"

#include "log.h"

#define TEST_CASE_NAME "random test"

static void Test1()
{
    {
        zysoft::default_random dr;
        for (int i = 0; i != 100; ++i) {
            auto n = dr.rand<std::size_t>(128, 512);
            std::string s = zysoft::gen_rand_str(dr, n);
            CHECK(s.size() == n);
            std::string s1 = s;
            dr.shuffle(s1.begin(), s1.end());
            CHECK(s != s1);
        }
    }
}

#if 1
TEST_CASE(TEST_CASE_NAME)
{
    LogInfo << TEST_CASE_NAME;
    try {
        Test1();
    }
    catch (const std::exception& e) {
        LogWarn << e.what();
        CHECK(0);
    }
}
#endif

#include <array>

#include <zysoft/zysoft/string/case_functions.h>
#include "../doctest/doctest.h"

#include "test_define.h"
#include "log.h"

#define TEST_CASE_NAME "case functions test"

// TODO 测试宽字符

static const std::string s_lower = "abcdefghijklmnopqrstuvwxyz";
static const std::string s_upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static 
void Test1()
{

    {
        std::string sl = s_lower;
        std::string su = s_upper;
            
        CHECK(sl == zysoft::to_lower(su));
        CHECK(su == zysoft::to_upper(sl));

        std::string sl_1(sl);
        std::string su_1(su);
        zysoft::make_upper(sl_1);
        zysoft::make_lower(su_1);
        CHECK(sl_1 == su);
        CHECK(su_1 == sl);
    }

    {
        // 不在A-Z a-z范围内
        std::string s;
        for (int i = 0; i <= 255; ++i) {
            s.push_back(static_cast<char>(i));
        }
        std::string sl = zysoft::to_lower(s);
        std::string su = zysoft::to_upper(s);

        for (int i = 0; i <= 255; ++i) {
            if (('A' <= i && i <= 'Z') || ('a' <= i && i <='z')) {
                char c = sl[i];
                CHECK(('a' <= c && c <= 'z'));
            } else {
                CHECK(s[i] == sl[i]);
            }
        }

        for (int i = 0; i <= 255; ++i) {
            if (('A' <= i && i <= 'Z') || ('a' <= i && i <='z')) {
                char c = su[i];
                CHECK(('A' <= c && c <= 'Z'));
            } else {
                CHECK(s[i] == su[i]);
            }
        }
    }

    // 其他容器
    {
        {
            std::array<char, 3> arr_lower = {'a', 'b', 'c'};
            std::array<char, 3> arr_upper = {'A', 'B', 'C'};
            CHECK(zysoft::to_lower(arr_upper) == arr_lower);
            CHECK(zysoft::to_upper(arr_lower) == arr_upper);

            std::array<char, 3> arr_lower_1 = arr_lower;
            std::array<char, 3> arr_upper_1 = arr_upper;
            CHECK(zysoft::make_upper(arr_lower_1) == arr_upper);
            CHECK(zysoft::make_lower(arr_lower_1) == arr_lower);
            CHECK(zysoft::make_upper(arr_upper_1) == arr_upper);
            CHECK(zysoft::make_lower(arr_upper_1) == arr_lower);
        }

        {
            std::vector<char> vec_lower(s_lower.begin(), s_lower.end());
            std::vector<char> vec_upper(s_upper.begin(), s_upper.end());
            CHECK(zysoft::to_lower(vec_upper) == vec_lower);
            CHECK(zysoft::to_upper(vec_lower) == vec_upper);

            std::vector<char> vec_lower_1 = vec_lower;
            std::vector<char> vec_upper_1 = vec_upper;
            CHECK(zysoft::make_upper(vec_lower_1) == vec_upper);
            CHECK(zysoft::make_lower(vec_lower_1) == vec_lower);
            CHECK(zysoft::make_upper(vec_upper_1) == vec_upper);
            CHECK(zysoft::make_lower(vec_upper_1) == vec_lower);
        }
    }


    // 注意：测试std::string_view 以下代码会在编译期出错
#if 0
    {
        std::string sl = s_lower;
        std::string su = s_upper;
        std::string_view sl_sv = sl;
        std::string_view su_sv = su;
        CHECK(zysoft::to_lower(su_sv) == s_lower);
        CHECK(zysoft::to_upper(sl_sv) == s_upper);
    }
#endif
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


#include <zysoft/zysoft/string/convert_functions.h>
#include "../doctest/doctest.h"

#include "test_define.h"
#include "log.h"

#define  TEST_CASE_NAME "convert functions test"

static void Test_String_To_Integral()
{
    LogInfo << "test string convert functions";
    {
        std::error_code ec{};
        // 前后空格
        {
            ec.clear();
            const char* s  = "   123";
            auto v1 =  zysoft::convert<int>::to_number(s, ec);
            CHECK(v1 == 123);
            CHECK(!ec); //LogInfo << ec.value() << " " << ec.message() << "\n";

            ec.clear();
            auto v2 = zysoft::convert<int, zysoft::skip_blank<false>>::to_number(s, ec);
            CHECK(v2 == 0);
            CHECK(ec.value() == (int)std::errc::invalid_argument); //LogInfo << ec.value() << " " << ec.message() << "\n";
            CHECK(zysoft::convert<int, zysoft::skip_blank<false>>::to_number_default(s, 11) == 11);

            ec.clear();
            s = "   123  ";
            auto v3 =  zysoft::convert<int>::to_number(s, ec);
            CHECK(v3 == 123);
            CHECK(!ec); //LogInfo << ec.value() << " " << ec.message() << "\n";

            ec.clear();
            s = "  123 4  ";
            auto v4 =  zysoft::convert<int>::to_number(s, ec);
            CHECK(v4 == 123);
            CHECK(ec.value() == 0); //LogInfo << ec.value() << " " << ec.message() << "\n";
        }

        // 溢出
        {
            ec.clear();
            std::string s = "65536";
            auto v3 = zysoft::convert<std::uint16_t>::to_number(s, ec);
            CHECK(v3 == 0);
            CHECK(ec.value() == (int)std::errc::result_out_of_range); //LogInfo << ec.value() << " " << ec.message() << "\n";
            CHECK(zysoft::convert<std::uint16_t>::to_number_default(s, 999) == 999);

            ec.clear();
            s = "65535";
            auto v4 = zysoft::convert<std::uint16_t>::to_number(s, ec);
            CHECK(v4 == 65535);
            CHECK(ec.value() == 0);

            // int
            ec.clear();
            s = "   2147483647  ";    // 2147483647 INT_MAX
            int v5 = zysoft::convert<int>::to_number(s, ec);
            CHECK(v5 == 2147483647);
            CHECK(ec.value() == 0);

            ec.clear();
            s = "   2147483648  ";    // 2147483648 INT_MAX + 1
            int v6 = zysoft::convert<int32_t>::to_number(s, ec);
            CHECK(v6 == 0);
            CHECK(ec.value() == (int)std::errc::result_out_of_range);

            ec.clear();
            s = "   2147483648  ";    // 2147483648 INT_MAX + 1
            auto v7 = zysoft::convert<std::int64_t>::to_number(s, ec);
            CHECK(v7 == 2147483648i64);
            CHECK(ec.value() == 0);

            // int64
            ec.clear();
            s = "   9223372036854775807  ";    // 9223372036854775807 LONGLONG_MAX
            auto v9 = zysoft::convert<long long int>::to_number(s, ec);
            CHECK(v9 == 9223372036854775807i64);
            CHECK(ec.value() == 0);

            ec.clear();
            s = "   9223372036854775808  ";    // 9223372036854775808 LONGLONG_MAX + 1
            auto v10 = zysoft::convert<long long int>::to_number(s, ec);
            CHECK(v10 == 0);
            CHECK(ec.value() != 0);
        }

        // 空字符串
        {
            ec.clear();
            std::string s = "";
            auto v4 = zysoft::convert<int>::to_number(s, ec);
            CHECK(v4 == 0);
            CHECK(ec.value() == (int)std::errc::invalid_argument); // LogInfo << ec.value() << " " << ec.message() << "\n";
            CHECK(zysoft::convert<int>::to_number_default(s, -1) == -1);
        }

        // 前面多个0
        {
            ec.clear();
            std::string s = "0000123456789";
            auto v5 = zysoft::convert<std::int32_t>::to_number(s, ec);
            CHECK(v5 == 123456789);
            CHECK(ec.value() == 0); // LogInfo << ec.value() << " " << ec.message() << "\n";
        }

        // 负数
        {
            ec.clear();
            std::string s = " -00001999 4 ";
            auto v5 = zysoft::convert<std::int32_t>::to_number(s, ec);
            CHECK(v5 == -1999);
            CHECK(ec.value() == 0); // LogInfo << ec.value() << " " << ec.message() << "\n";

            ec.clear();
            s = "   - 1  ";
            auto v6 = zysoft::convert<std::int32_t>::to_number(s, ec);
            CHECK(v6 == 0);
            CHECK(ec.value() == (int)std::errc::invalid_argument); // LogInfo << ec.value() << " " << ec.message() << "\n";

            // int32
            ec.clear();
            s = "   -000002147483648  ";    // -2147483648 INT_MIN
            std::int32_t v100 = zysoft::convert<std::int32_t>::to_number(s, ec);
            const int INT_MIN_XX = -2147483647 - 1;
            CHECK(v100 == INT_MIN_XX);
            CHECK(ec.value() == 0);

            ec.clear();
            s = "   -000002147483649  ";    // -2147483649 INT_MIN - 1
            auto v7 = zysoft::convert<std::int32_t>::to_number(s, ec);
            CHECK(v7 == 0);
            CHECK(ec.value() == (int)std::errc::result_out_of_range);

            ec.clear();
            s = "   -000002147483649  ";    // -2147483649 INT_MIN - 1
            auto v8 = zysoft::convert<std::int64_t>::to_number(s, ec);
            CHECK(v8 == -2147483649i64);
            CHECK(ec.value() == 0);

            // int64
            ec.clear();
            s = "   -9223372036854775808  ";    // -9223372036854775808 LONGLONG_MIN
            const long long int LONGLONG_MIN_XX = -9223372036854775807i64 - 1;
            auto v9 = zysoft::convert<long long int>::to_number(s, ec);
            CHECK(v9 == LONGLONG_MIN_XX);
            CHECK(ec.value() == 0);

            ec.clear();
            s = "   -9223372036854775809  ";    // -9223372036854775809 LONGLONG_MIN - 1
            auto v10 = zysoft::convert<long long int>::to_number(s, ec);
            CHECK(v10 == 0);
            CHECK(ec.value() != 0);
        }

    }
}

static void Test_String_To_Float()
{
    LogInfo << "test string convert functions float double long double";
    // 注意：浮点数存在精度问题，以下测试并不保证一定正确。转换后的值会在近似范围内。
    {
        const char* s  = " 3.1415926";
        std::error_code ec{};
        LogInfo <<zysoft::convert<float>::to_number(s, ec);
        //LogInfo << ec.value() << " " << ec.message() << "\n";

        ec.clear();
        LogInfo <<zysoft::convert<float, zysoft::skip_blank<false>>::to_number(s, ec);
        //LogInfo << ec.value() << " " << ec.message() << "\n";
        //CHECK(zysoft::convert<char, zysoft::skip_blank<false>>::to_number_default(s, 11) == 11);

        s = "-65536.123";
        ec.clear();
        LogInfo << zysoft::convert<float>::to_number(s, ec);
        //LogInfo << ec.value() << " " << ec.message() << "\n";
    }
}

#if 1
TEST_CASE(TEST_CASE_NAME)
{
    LogInfo << TEST_CASE_NAME;
    try {
        Test_String_To_Integral();
        Test_String_To_Float();
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n";
        CHECK(false);
    }
}
#endif 

#include <zysoft/zysoft/utility/cmdargs.h>

#include "../doctest/doctest.h"

#include "log.h"

#define TEST_CASE_NAME "cmdargs test"

using namespace zysoft;

static void Test1(int argc, char** argv)
{
    for (int i = 0; i != argc; ++i) {
        std::cout << i << ": " << argv[i] << " len: " << std::strlen(argv[i]) << "\n";
    }
}

static void Test2(int argc, char** argv)
{
    //typedef basic_cmdargs<std::string, std::string> cmdargs;
    typedef basic_cmdargs cmdargs;
    cmdargs args(argc, argv);
    cmdargs::option opt;
    CHECK(args.has_option("name"));
    CHECK(args.has_option("daemon"));
    CHECK(!args.has_option("help"));
}

#if 1
TEST_CASE(TEST_CASE_NAME)
{
    LogInfo << TEST_CASE_NAME;
    try {
        std::vector<char*> argv =
        {
            (char*)"x.exe",
            (char*)"--name=\"abcd 1234 sfo\"",
            (char*)"--daemon",
        };

        Test2((int)argv.size(), argv.data());
    } catch (const std::exception& e) {
        LogWarn << e.what();
        CHECK(0);
    }
}
#endif

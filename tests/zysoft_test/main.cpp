#include <cassert>
#include <vector>

#include "test_define.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "../doctest/doctest.h"

#define MA 0
#define MB 0
#define MC 0

int main(int argc, char** argv)
{
#if defined(_MSC_VER)
    //system("chcp 65001");
#endif

#if MA
    std::cout << "MA: " << MA << "\n";
#elif MB
    std::cout << "MB: " << MB << "\n";
#elif MC
    std::cout << "MC: " << MC << "\n";
#else
    std::cout << "MX: \n";
#endif

    if (0) {
        std::cout << "__cplusplus:  " << __cplusplus << "\n";
        std::cout << "ZYSOFT_CXX:   " << ZYSOFT_CXX << "\n";
        std::cout << "ZYSOFT_MSVC:  " << ZYSOFT_MSVC << "\n";
        std::cout << "ZYSOFT_CLANG: " << ZYSOFT_CLANG << "\n";
        std::cout << "ZYSOFT_GCC:   " << ZYSOFT_GCC << "\n";

#if (ZYSOFT_MSVC > 0)
        std::cout << "_MSC_VER:         " << _MSC_VER << "\n";
        std::cout << "_MSC_FULL_VER:    " << _MSC_FULL_VER << "\n";
        std::cout << "_MSVC_LANG:       " << _MSVC_LANG << "\n";
#endif
        return 0;
    }

    doctest::Context context;
    context.applyCommandLine(argc, argv);
    int res = context.run(); // run doctest
    // important - query flags (and --exit) rely on the user doing this
    if (context.shouldExit()) {
        // propagate the result of the tests
        return res;
    }
    return EXIT_SUCCESS;
}

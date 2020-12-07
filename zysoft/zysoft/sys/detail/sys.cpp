#include <zysoft/zysoft/sys/sys.h>

#if defined(_WIN32)
#include <stdio.h>
#endif // _WIN32

namespace zysoft 
{

void fopen(std::FILE** f, const char* fname, const char* mod)
{
#if defined(_WIN32)
    ::fopen_s(f, fname, mod);
#else
    std::FILE* ff = std::fopen(fname, mod);
    if (ff) {
        *f = ff;
    }
#endif
}

std::FILE* fopen(const char* fname, const char* mod)
{
    std::FILE* f = nullptr;
    fopen(&f, fname, mod);
    return f;
}

} // namespace zysoft


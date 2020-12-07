#ifndef _ZYSOFT_ZYSOFT_SYS_SYS_H
#define _ZYSOFT_ZYSOFT_SYS_SYS_H

#include <cstdio>

namespace zysoft 
{

void fopen(std::FILE** f, const char* fname, const char* mod);
std::FILE* fopen(const char* fname, const char* mod);

} // namespace zysoft

#endif // !_ZYSOFT_ZYSOFT_SYS_SYS_H

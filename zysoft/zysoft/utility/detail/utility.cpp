#include <zysoft/zysoft_fwd.h>
#include <cstring>
#include <type_traits>
#include <memory>
#include <cstdio>
#include <sstream>

#include <zysoft/zysoft/utility/utility.h>
#include <zysoft/zysoft/sys/sys.h>
#include <zysoft/zysoft/time/time.h>

namespace zysoft
{

std::string cat_file(const char* f)
{
    std::FILE* fp = fopen(f, "rb");
    if (!fp)
        return "";

    std::string content;
    std::array<char, 1024 * 4> buffer{};
    while (1) {
        std::size_t readn = std::fread(buffer.data(), 1, buffer.size(), fp);
        if (readn == 0)
            break;
        content.append(buffer.data(), buffer.data() + readn);
    }
    std::fclose(fp);
    return content;
}

void filter_comment(const std::string& src, std::string& dst)
{
    std::istringstream istm{src};
    std::ostringstream ostm{};
    std::string str;
    bool comment = false;
    while (!istm.eof()) {
        std::getline(istm, str);
        comment = false;
        for (const auto& c : str) {
            if (::isblank(c))
                continue;
            comment = c == '#';
            break;
        }
        if (!comment)
            ostm << str << "\n";
    }
    dst = ostm.str();
}

std::int64_t unix_time_milliseconds(const struct timeval* tv)
{
    if (tv) {
        return std::int64_t(tv->tv_sec)*1000 + std::int64_t(tv->tv_usec)/1000;
    }
    struct timeval tmp{};
    gettimeofday(&tmp);
    tv = &tmp;
    return std::int64_t(tv->tv_sec)*1000 + std::int64_t(tv->tv_usec)/1000;
}

std::int64_t unix_time_microseconds(const struct timeval* tv)
{
    if (tv) {
        return std::int64_t(tv->tv_sec)*1000000 + std::int64_t(tv->tv_usec);
    }
    struct timeval tmp{};
    gettimeofday(&tmp);
    tv = &tmp;
    return std::int64_t(tv->tv_sec)*1000000 + std::int64_t(tv->tv_usec);
}

std::int32_t cat_file(const std::string& path, std::string* out)
{
    std::FILE* f = fopen(path.c_str(), "rb");
    if (!f) {
        return -1;
    }

    std::string content;
    std::array<char, 1024 * 4> buffer;
    std::uint64_t total = 0;
    while (true) {
        std::size_t readn = std::fread(buffer.data(), 1, buffer.size(), f);
        if (readn == 0) {
            break;
        }
        total += readn;
        content.append(buffer.data(), buffer.data() + readn);
    }
    std::fclose(f);
    std::swap(*out, content);
    return static_cast<std::int32_t>(out->size());
}

std::size_t str_replace(std::string* str, char src, char dest)
{
    size_t t = 0;
    std::transform(str->begin(), str->end(), str->begin(),
        [&t, src, dest](char& c)
        {
            if (c == src) {
                c = dest;
                ++t;
            }
            return c;
        } );
    return t;
}

std::size_t str_remove(std::string* str, char src)
{
    auto it = std::remove(str->begin(), str->end(), src);
    auto n = std::distance(it, str->end());
    str->erase(it, str->end());
    return n;
}

} // namespace zysoft


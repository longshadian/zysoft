#include <zysoft/zysoft/utility/utility.h>

#include <zysoft/zysoft/sys/sys.h>

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


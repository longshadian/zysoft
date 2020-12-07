#ifndef _ZYSOFT_ZYSOFT_UTILITY_UTILITY_H
#define _ZYSOFT_ZYSOFT_UTILITY_UTILITY_H

#include <cstring>  // std::memset
#include <cstdio>

#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace zysoft
{

template< typename I
        , typename O
        >
inline void hex_dump(I b, I e, O o)
{
    static const std::uint8_t hex[] = "0123456789abcdef";
    while (b != e) {
        *o++ = hex[*b >> 4];
        *o++ = hex[*b++ & 0xf];
    }
}

template<typename T>
inline void bzero(T* t)
{
    static_assert(std::is_pod<T>::value, "T must be pod!");
    std::memset(t, 0, sizeof(T));
}

template <typename E>
inline std::underlying_type_t<E> enum_value(E e)
{
    //static_assert(std::is_enum<E>::value, "E must be enum or enum class !");
    return static_cast<std::underlying_type_t<E>>(e);
}

std::string cat_file(const char* f);
std::int32_t cat_file(const std::string& path, std::string* out);
std::size_t str_replace(std::string* str, char src, char dest);
std::size_t str_remove(std::string* str, char src);

} // namespace zysoft

#endif // !_ZYSOFT_ZYSOFT_UTILITY_UTILITY_H

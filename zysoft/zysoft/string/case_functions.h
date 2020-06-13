#pragma once

#include <cctype>
#include <algorithm>
#include <string_view>

namespace zysoft
{

template <typename S>
inline S& make_upper(S& s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](auto c) { return ::toupper(c); });
    return s;
}

template <typename S>
inline S& make_lower(S& s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](int c) { return ::tolower(c); });
    return s;
}

template <typename S>
inline S to_upper(const S& s)
{
    S r(s);
    make_upper(r);
    return r;
}

template <typename S>
inline S to_lower(const S& s)
{
    S r(s);
    make_lower(r);
    return r;
}

template <>
inline std::string_view to_upper(const std::string_view&) = delete;

template <>
inline std::string_view to_lower(const std::string_view&) = delete;

} // namespace zysoft


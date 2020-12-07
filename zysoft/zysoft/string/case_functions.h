#ifndef _ZYSOFT_ZYSOFT_STRING_CASE_FUNCTIONS_H
#define _ZYSOFT_ZYSOFT_STRING_CASE_FUNCTIONS_H

#include <cctype>
#include <algorithm>

#include <zysoft/zysoft_fwd.h>

#if (ZYSOFT_CXX >= ZYSOFT_CXX17)
#include <string_view>
#endif

namespace zysoft
{

template <typename S>
inline S& make_upper(S& s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](int c) { return ::toupper(c); });
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

#if (ZYSOFT_CXX >= ZYSOFT_CXX17)
template <>
inline std::string_view to_upper(const std::string_view&) = delete;

template <>
inline std::string_view to_lower(const std::string_view&) = delete;
#endif

} // namespace zysoft

#endif // !_ZYSOFT_ZYSOFT_STRING_CASE_FUNCTIONS_H

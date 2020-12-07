#ifndef _ZYSOFT_ZYSOFT_SYS_TIME_H
#define _ZYSOFT_ZYSOFT_SYS_TIME_H

#include <ctime>

#include <zysoft/zysoft_fwd.h>

namespace zysoft
{

inline void gmtime(const std::time_t* t, struct tm* output)
{
#if defined(ZYSOFT_MSVC)
    ::gmtime_s(output, t);
#else
    ::gmtime_r(t, output);
#endif
}

inline void gettimeofday(struct timeval* tp)
{
#if defined(ZYSOFT_MSVC)
    std::uint64_t intervals;
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);

    /*
     * A file time is a 64-bit value that represents the number
     * of 100-nanosecond intervals that have elapsed since
     * January 1, 1601 12:00 A.M. UTC.
     *
     * Between January 1, 1970 (Epoch) and January 1, 1601 there were
     * 134744 days,
     * 11644473600 seconds or
     * 11644473600,000,000,0 100-nanosecond intervals.
     *
     * See also MSKB Q167296.
     */

    intervals = ((std::uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    intervals -= 116444736000000000;

    tp->tv_sec = (long)(intervals / 10000000);
    tp->tv_usec = (long)((intervals % 10000000) / 10);
#else
    ::gettimeofday(tp, nullptr);
#endif // defined(_WIN32)
}

inline struct tm* localtime(const time_t* t, struct tm* output)
{
#if defined(ZYSOFT_MSVC)
    ::localtime_s(output, t);
#else
    ::localtime_r(t, output);
#endif
    return output;
}

inline std::int64_t timeval_to_milliseconds(const struct timeval* tv)
{
    std::int64_t v = tv->tv_sec * std::int64_t(1000);
    v += (tv->tv_usec + std::int64_t(999)) / 1000;
    return v;
}

inline std::int64_t timeval_to_milliseconds(const struct timeval& tv)
{
    return timeval_to_milliseconds(&tv);
}

inline std::int64_t timeval_to_microseconds(const struct timeval* tv)
{
    std::int64_t v = std::int64_t(tv->tv_sec) * 1000000 + tv->tv_usec;
    return v;
}

inline std::int64_t timeval_to_microseconds(const struct timeval& tv)
{
    return timeval_to_microseconds(&tv);
}

inline struct timeval* milliseconds_to_timeval(std::int64_t milliseconds, struct timeval* tv)
{
    ZYSOFT_ASSERT(milliseconds >= 0);
    tv->tv_sec = static_cast<decltype(tv->tv_sec)>(milliseconds / 1000);
    tv->tv_usec = static_cast<decltype(tv->tv_usec)>((milliseconds % 1000) * 1000);
    return tv;
}

inline struct timeval* microseconds_to_timeval(std::int64_t microseconds, struct timeval* tv)
{
    ZYSOFT_ASSERT(microseconds >= 0);
    tv->tv_sec = static_cast<decltype(tv->tv_sec)>(microseconds / 1000000);
    tv->tv_usec = static_cast<decltype(tv->tv_usec)>(microseconds % 1000000);
    return tv;
}

inline std::int64_t utc_milliseconds()
{
    struct timeval tv{};
    gettimeofday(&tv);
    return timeval_to_milliseconds(tv);
}

inline std::int64_t utc_microseconds()
{
    struct timeval tv{};
    gettimeofday(&tv);
    return timeval_to_microseconds(tv);
}


} // namespace zysoft

#endif // !_ZYSOFT_ZYSOFT_SYS_TIME_H


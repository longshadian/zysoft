#ifndef _ZYSOFT_ZYSOFT_TIMER_TIMER_H
#define _ZYSOFT_ZYSOFT_TIMER_TIMER_H

#include <cstdint>
#include <chrono>

#include <zysoft/zysoft/zysoft_fwd.h>

namespace zysoft
{

template <typename T>
class clock_timer
{
public:
    using clock_type = T;
    using time_point_type = typename T::time_point;

    clock_timer()
        : start_(), end_()
    {
        reset();
    }

    ~clock_timer() = default;
    clock_timer(const clock_timer&) = default;
    clock_timer& operator=(const clock_timer&) = default;

    void reset()
    {
        start_ = clock_type::now();
        end_ = start_;
    }

    void stop()
    {
        end_ = clock_type::now();
    }

    template <typename D>
    D cost() const
    {
        return std::chrono::duration_cast<D>(end_ - start_);
    }

    std::int64_t microseconds() const
    {
        return cost<std::chrono::microseconds>().count();
    }

    std::int64_t milliseconds() const
    {
        return cost<std::chrono::milliseconds>().count();
    }

    std::int64_t seconds() const
    {
        return cost<std::chrono::seconds>().count();
    }

    time_point_type start_;
    time_point_type end_;
};

using steady_performace_timer = clock_timer<std::chrono::steady_clock>;

template <typename T>
class basic_timer
{
public:
    using Duration = T;

public:
    basic_timer() = default;
    ~basic_timer() = default;
    basic_timer(const basic_timer&) = default;
    basic_timer& operator=(const basic_timer&) = default;

    template <typename D>
    explicit basic_timer(D d)
        : flag_(), start_(), expire_(d)
    {
    }

    void update(Duration delta)
    {
        start_ += delta;
    }

    bool passed() const
    {
        return expire_ <= start_;
    }

    void reset()
    {
        start_ = Duration::zero();
        flag_ = 0;
    }

    Duration remain() const
    {
        return expire_ - start_;
    }

    void flag(std::int64_t flag)
    {
        flag_ = flag;
    }

    std::int64_t flag() const
    {
        return flag_;
    }

    std::int64_t    flag_;
    Duration        start_;
    Duration        expire_;
};
using milliseconds_timer = basic_timer<std::chrono::milliseconds>;

} // namespace zysoft

#endif // !_ZYSOFT_ZYSOFT_TIMER_TIMER_H


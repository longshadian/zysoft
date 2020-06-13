#pragma once

#include <boost/asio.hpp>

#include <zysoft/zysoft/timer/timing_wheel.h>

#include <zysoft/experimental/net/tcp_connection.h>

namespace zysoft
{
namespace net
{


class timer_queue
{
public:
    static constexpr int queue_size = 10; 
    static constexpr int tick_duration_seconds = 1;

    struct timer_handle;
    using timer_handle_ptr = std::shared_ptr<timer_handle>;

    using timer_callback = std::function<void(std::uint64_t)>;
    using wheel_type = timing_wheel<std::shared_ptr<timer_handle>, queue_size>;

    struct timer_handle
    {
        friend class timer_queue;
    private:
        timer_callback cb_;
        wheel_type::wheel_handle hdl_;
    };

public:
    timer_queue(boost::asio::io_context& io_ctx)
        : wheel_()
        , timer_()
        , expired_()
    {
        timer_ = std::make_shared<boost::asio::steady_timer>(io_ctx.get_executor());
        reg_timer();
    }

    ~timer_queue()
    {
    }

    timer_queue(const timer_queue&) = delete;
    timer_queue& operator=(const timer_queue&) = delete;
    timer_queue(timer_queue&&) = delete;
    timer_queue& operator=(timer_queue&&) = delete;

    timer_handle_ptr add_timer(timer_callback cb, std::uint64_t sec)
    {
        auto p = std::make_shared<timer_handle>();
        p->cb_ = std::move(cb);
        p->hdl_ = wheel_.add(p, sec);
        return p;
    }

    void reset_timer(const timer_handle_ptr& p)
    {
        wheel_.reset(p->hdl_);
    }

    void cancel_timer(const timer_handle_ptr& p)
    {
        wheel_.cancel(p->hdl_);
    }

private:
    void handle_on_timer(boost::system::error_code ec)
    {
        if (ec) {
        } else {
            turn_wheel();
        }
        reg_timer();
    }

    void reg_timer()
    {
        timer_->expires_from_now(std::chrono::seconds(tick_duration_seconds));
        timer_->async_wait(std::bind(&timer_queue::handle_on_timer, this, std::placeholders::_1));
    }

    void turn_wheel()
    {
        wheel_.tick();
        wheel_.get_expired(expired_);
        for (auto& p : expired_) {
            try {
                p.first->cb_(p.second);
            } catch (...) {
            }
        }
        expired_.clear();
    }

private:
    wheel_type  wheel_;
    using timer_ptr = std::shared_ptr<boost::asio::steady_timer>;
    timer_ptr timer_;
    std::vector<std::pair<timer_handle_ptr, std::uint64_t>> expired_;
};

} // namespace net
} // namespace zysoft



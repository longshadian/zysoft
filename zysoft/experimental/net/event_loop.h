#pragma once

#include <atomic>
#include <cassert>
#include <functional>
#include <thread>

#include <zysoft/experimental/net/types.h>
#include <zysoft/experimental/net/timer_queue.h>
#include <zysoft/experimental/net/console_log.h>

namespace zysoft
{
namespace net
{

class event_loop
{
public:
    using functor = std::function<void()>;

public:
    event_loop()
        : io_ctx_()
        , looping_()
        , thread_id_(std::this_thread::get_id())
        , timer_queue_(std::make_unique<timer_queue>(io_ctx_))
    {
    }

    ~event_loop()
    {
        quit();
    }

    event_loop(const event_loop&) = delete;
    event_loop& operator=(const event_loop&) = delete;
    event_loop(event_loop&&) = delete;
    event_loop& operator=(event_loop&&) = delete;

    void loop()
    {
        assert(!looping_);
        looping_ = true;

        boost::asio::io_context::work work_guard{io_ctx_};
        while (looping_) {
            try {
                io_ctx_.run();
                NET_LOG_TRACE << "io_context exit";
                break;
            } catch (const std::exception& e) {
                NET_LOG_TRACE << "io_content exception: " << e.what();
                io_ctx_.restart();
            }
        }
    }

    void quit()
    {
        looping_ = false;
        io_ctx_.stop();
    }

    void run_in_loop(functor f)
    {
        io_ctx_.post(std::move(f));
    }

    boost::asio::io_context& get_io_context()
    {
        return io_ctx_;
    }

    bool is_in_loop_thread() const
    {
        return thread_id_ == std::this_thread::get_id();
    }

    void assert_run_in_loop()
    {
        // TODO add log
        assert(is_in_loop_thread());
    }

    timer_queue* get_timer_queue()
    {
        return timer_queue_.get();
    }

private:
    boost::asio::io_context io_ctx_;
    std::atomic<bool> looping_;
    const std::thread::id thread_id_;
    std::unique_ptr<timer_queue> timer_queue_;
};

} // namespace net
} // namespace zysoft



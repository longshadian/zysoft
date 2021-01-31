#ifndef _ZYSOFT_NET_EVENT_LOOP_H
#define _ZYSOFT_NET_EVENT_LOOP_H

#include <atomic>
#include <cassert>
#include <functional>
#include <thread>

#include <zysoft/net/types.h>
#include <zysoft/net/timer_queue.h>
#include <zysoft/net/log.h>

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
        : io_ctx_{}
        , thread_id_{ std::this_thread::get_id() }
        , work_guard_{io_ctx_.get_executor()}
        , timer_queue_{ std::make_unique<timer_queue>(io_ctx_) }
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
        while (1) {
            try {
                io_ctx_.run();
                ZYSOFT_NET_TRACE("io_context exit");
                break;
            } catch (const std::exception& e) {
                ZYSOFT_NET_WARN("io_content exception: %s", e.what());
                //io_ctx_.restart();
            }
        }
    }

    void quit()
    {
        work_guard_.reset();
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
    const std::thread::id thread_id_;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_;
    std::unique_ptr<timer_queue> timer_queue_;
};

} // namespace net
} // namespace zysoft

#endif // !_ZYSOFT_NET_EVENT_LOOP_H

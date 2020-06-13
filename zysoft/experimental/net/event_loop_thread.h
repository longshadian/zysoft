#pragma once

#include <atomic>
#include <memory>
#include <thread>
#include <functional>

namespace zysoft
{
namespace net
{

class event_loop;

class event_loop_thread
{
public:
    using thread_run_callback = std::function<void(event_loop_thread*)>;
public:
    event_loop_thread(thread_run_callback cb = thread_run_callback())
        : running_()
        , callback_(std::move(cb))
        , loop_()   // 不能在此创建，必须在线程函数中创建此对象
        , thread_()
    {
    }

    ~event_loop_thread()
    {
        if (running_ && loop_) {
            loop_->quit();
        }
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    event_loop_thread(const event_loop_thread&) = delete;
    event_loop_thread& operator=(const event_loop_thread&) = delete;
    event_loop_thread(event_loop_thread&&) = delete;
    event_loop_thread& operator=(event_loop_thread&&) = delete;

    void start()
    {
        if (running_.exchange(true)) {
            return;
        }
        thread_ = std::thread(std::bind(&event_loop_thread::thread_func, this));
    }

    event_loop* get_event_loop()
    {
        return loop_.get();
    }

private:
    void thread_func()
    {
        if (callback_) 
            callback_(this);

        loop_ = std::make_unique<event_loop>();
        loop_->loop();
    }

private:
    std::atomic<bool>   running_;
    thread_run_callback callback_;
    std::unique_ptr<event_loop> loop_;
    std::thread thread_;
};

} // namespace net
} // namespace zysoft


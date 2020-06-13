#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>

#include <zysoft/experimental/net/event_loop_thread.h>

namespace zysoft
{
namespace net
{

class event_loop;
class event_loop_thread;

class event_loop_thread_pool
{
public:
    event_loop_thread_pool(event_loop* base_loop)
        : base_loop_(base_loop)
        , threads_()
        , num_threads_(0)
        , next_(0)
        , running_num_(0)
    {
    }
    
    ~event_loop_thread_pool()
    {
    }

    event_loop_thread_pool(const event_loop_thread_pool&) = delete;
    event_loop_thread_pool& operator=(const event_loop_thread_pool&) = delete;
    event_loop_thread_pool(event_loop_thread_pool&&) = delete;
    event_loop_thread_pool& operator=(event_loop_thread_pool&&) = delete;

    void thread_num(int num) { num_threads_ = num; }
    int thread_num() const { return num_threads_; }

    void sync_start()
    {
        async_start();
        while (running_num_ < num_threads_) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void async_start()
    {
        for (int i = 0; i < num_threads_; ++i) {
            auto thd = std::make_unique<event_loop_thread>(std::bind(&event_loop_thread_pool::thread_cb, this, std::placeholders::_1));
            thd->start();
            threads_.emplace_back(std::move(thd));
        }
    }

    event_loop* next_event_loop()
    {
        if (threads_.empty()) {
            return base_loop_;
        }
        event_loop* loop = threads_[next_]->get_event_loop();
        ++next_;
        if (next_ >= threads_.size()) {
            next_ = 0;
        }
        return loop;
    }
private:
    void thread_cb(event_loop_thread* /* thd */)
    {
        ++running_num_;
    }

private:
    event_loop* base_loop_;
    std::vector<std::unique_ptr<event_loop_thread>> threads_;
    int num_threads_;
    std::size_t next_;
    std::atomic<int> running_num_;
};


} // namespace net
} // namespace zysoft


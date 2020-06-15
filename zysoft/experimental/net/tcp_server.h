#pragma once

#include <memory>
#include <unordered_map>

#include <boost/asio.hpp>

#include <zysoft/experimental/net/types.h>
#include <zysoft/experimental/net/acceptor.h>
#include <zysoft/experimental/net/event_loop.h>
#include <zysoft/experimental/net/event_loop_thread_pool.h>
#include <zysoft/experimental/net/inet_address.h>
#include <zysoft/experimental/net/utility.h>

namespace zysoft
{
namespace net
{

class acceptor;
class event_loop;
class event_loop_thread_pool;

class tcp_server
{
public:
    explicit
    tcp_server(event_loop* loop, const inet_address& addr)
        : loop_(loop)
        , acceptor_(std::make_unique<acceptor>(loop_, addr))
        , thread_pool_(std::make_unique<event_loop_thread_pool>(loop_))
        , connection_cb_(std::bind(&util::default_connection_cb, std::placeholders::_1))
        , read_cb_(std::bind(&util::default_message_cb, std::placeholders::_1, std::placeholders::_2))
        , write_complete_cb_(std::bind(&util::default_write_complete_cb, std::placeholders::_1, std::placeholders::_2))
        , close_cb_(std::bind(&util::default_close_cb, std::placeholders::_1))
        , error_cb_(std::bind(&util::default_error_cb, std::placeholders::_1, std::placeholders::_2))
        , timeout_cb_(std::bind(&util::default_timeout_cb, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
        , connections_()
        , nextConnId_(1)
        , read_timeout_(0)
        , write_timeout_(0)
    {
    }

    ~tcp_server() {}

    tcp_server(const tcp_server& rhs) = delete;
    tcp_server& operator=(const tcp_server& rhs) = delete;
    tcp_server(tcp_server&& rhs) = delete;
    tcp_server& operator=(tcp_server&& rhs) = delete;

    void set_connection_callback(connection_callback cb) { connection_cb_ = std::move(cb); }
    void set_read_callback(read_callback cb) { read_cb_ = std::move(cb); }
    void set_write_complete_callback(write_complete_callback cb) { write_complete_cb_ = std::move(cb); } 
    void set_close_callback(close_callback cb) { close_cb_ = std::move(cb); }
    void set_error_callback(error_callback cb) { error_cb_ = std::move(cb); }
    void set_timeout_callback(timeout_callback cb) { timeout_cb_ = std::move(cb); }
    void set_reuse_addr(bool b) { acceptor_->reuse_addr(b); }
    void set_thread_num(int num) { thread_pool_->thread_num(num); }
    void set_read_timeout(std::uint64_t sec) { read_timeout_ = sec; }
    void set_write_timeout(std::uint64_t sec) { write_timeout_ = sec; }

    bool start()
    {
        thread_pool_->sync_start();
        if (!acceptor_->listen()) {
            return false;
        }
        start_accept();
        return true;
    }

private:
 
    void start_accept()
    {
        event_loop* io_loop = thread_pool_->next_event_loop();
        std::string conn_name = util::create_connection_name(acceptor_->GetAddr(), nextConnId_);
        tcp_connection_ptr conn = tcp_connection::create(io_loop, std::move(conn_name));
        conn->set_connection_callback(connection_cb_);
        conn->set_read_callback(read_cb_);
        conn->set_write_complete_callback(write_complete_cb_);
        conn->set_cleanup_callback(std::bind(&tcp_server::handle_connection_cleanup, this, std::placeholders::_1, std::placeholders::_2));
        conn->set_read_timeout_duration(read_timeout_);
        conn->set_write_timeout_duration(write_timeout_);
        ++nextConnId_;
        acceptor_->wait_accept(conn, std::bind(&tcp_server::handle_new_connection, this, std::placeholders::_1, std::placeholders::_2));
    }

    void handle_new_connection(tcp_connection_ptr conn, const boost::system::error_code& ec)
    {
        if (ec) {
            NET_LOG_WARN << "Acceptor error. ecode: " << ec.value();
            acceptor_->wait_accept(nullptr, std::bind(&tcp_server::handle_new_connection, this, std::placeholders::_1, std::placeholders::_2));
            return;
        }
        conn->set_state(tcp_connection::EState::Connecting);
        connections_[conn->get_name()] = conn;
        conn->get_event_loop()->run_in_loop(std::bind(&tcp_connection::connect_establised, conn));
        start_accept();
    }

    void handle_connection_cleanup(const tcp_connection_ptr& conn, const detail::cleanup_context& ctx)
    {
        if (ctx.type_ == detail::cleanup_type::timeout) {
            // 超时
            try {
                timeout_cb_(conn, ctx.timeout_type_, ctx.timeout_sec_);
            } catch (...) {
            }
        } else {
            // 用户调用shutdown/force_close 
            if (ctx.call_error_cb_) {
                try {
                    if (ctx.err_code_.value() == boost::asio::error::eof) {
                        close_cb_(conn);
                    } else {
                        error_cb_(conn, ctx.err_code_);
                    }
                } catch (...) { 
                }
            }
        }
        loop_->run_in_loop(std::bind(&tcp_server::remove_connection, this, conn));
    }

    void remove_connection(const tcp_connection_ptr& conn)
    {
        const std::string& name = conn->get_name();
        NET_LOG_TRACE << "TcpServer remove connection: " << name;
        std::size_t n = connections_.erase(name);
        assert(n == 1);

        conn->get_event_loop()->run_in_loop(std::bind(&tcp_connection::connect_destroy, conn));
    }

private:
    event_loop* loop_;
    std::unique_ptr<acceptor> acceptor_;
    std::unique_ptr<event_loop_thread_pool> thread_pool_;

    connection_callback         connection_cb_;
    read_callback               read_cb_;
    write_complete_callback     write_complete_cb_;
    close_callback              close_cb_;
    error_callback              error_cb_;
    timeout_callback            timeout_cb_;

    using ConnectionMap = std::unordered_map<std::string, tcp_connection_ptr>;
    ConnectionMap           connections_;
    int                     nextConnId_;
    std::uint64_t           read_timeout_;
    std::uint64_t           write_timeout_;
};


} // namespace net
} // namespace zysoft



#pragma once

#include <functional>
#include <memory>

#include <zysoft/experimental/net/types.h>
#include <zysoft/experimental/net/inet_address.h>
#include <zysoft/experimental/net/utility.h>
#include <zysoft/experimental/net/console_log.h>

namespace zysoft
{
namespace net
{

class event_loop;

class acceptor
{
public:
    using accept_callback = std::function<void(tcp_connection_ptr& conn, const boost::system::error_code& ec)>;
public:
    acceptor(event_loop* loop, inet_address addr)
        : loop_(loop)
        , addr_(addr)
        , acceptor_(std::make_unique<boost::asio::ip::tcp::acceptor>(loop_->get_io_context().get_executor()))
        , conn_()
        , accept_cb_()
        , reuse_addr_()
    {
    }

    ~acceptor()
    {
    }

    acceptor(const acceptor&) = delete;
    acceptor& operator=(const acceptor&) = delete;
    acceptor(acceptor&&) = delete;
    acceptor& operator=(acceptor&&) = delete;

    void reuse_addr(bool b) { reuse_addr_ = b; }
    bool resue_addr() { return reuse_addr_; }
    const inet_address& GetAddr() const { return addr_; }

    bool listen(std::string* err_msg = nullptr)
    {
        try {
            auto ep = util::create_endpoint(addr_.ip_, addr_.port_);
            acceptor_->open(ep.protocol());
            if (reuse_addr_)
                acceptor_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            acceptor_->bind(ep);
            acceptor_->listen();
            NET_LOG_TRACE << "init acceptor success listening " << addr_.ip_port_to_string();
            return true;
        } catch (const std::exception& e) {
            if (err_msg) {
                *err_msg = e.what();
            }
            //NET_LOG_CRIT << "init acceptor error. " << addr_.IpPort_ToString() << " reason: " << e.what();
            return false;
        }
    }

    void wait_accept(tcp_connection_ptr conn, accept_callback cb)
    {
        conn_ = conn;
        accept_cb_ = std::move(cb);
        acceptor_->async_accept(conn_->get_socket(),
            [this](const boost::system::error_code& ec) 
            {
                accept_cb_(conn_, ec);
            }
        );
    }

private:
    event_loop* loop_;
    inet_address addr_;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    tcp_connection_ptr conn_;
    accept_callback accept_cb_;
    bool reuse_addr_;
};

} // namespace net
} // namespace zysoft


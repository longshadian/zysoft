#pragma once

#include <zysoft/experimental/net/types.h>
#include <zysoft/experimental/net/event_loop.h>
#include <zysoft/experimental/net/connector.h>
#include <zysoft/experimental/net/inet_address.h>
#include <zysoft/experimental/net/tcp_connection.h>

namespace zysoft
{
namespace net
{

class tcp_client
{
public:
    tcp_client(event_loop* loop, const inet_address& addr)
        : loop_(loop)
        , addr_(addr)
        //, connection_cb_(std::bind(&util::default_close_cb, std::placeholders::_1))
        //, message_cb_(std::bind(&util::default_message_cb, std::placeholders::_1, std::placeholders::_2))
        //, write_complete_cb_(std::bind(&util::default_write_complete_cb, std::placeholders::_1, std::placeholders::_2))
        , close_cb_(std::bind(&util::default_close_cb, std::placeholders::_1))
        , error_cb_(std::bind(&util::default_error_cb, std::placeholders::_1, std::placeholders::_2))
        , conn_(std::make_unique<connector>(loop_, addr_))
    {
        conn_->get_connection()->set_connection_callback(std::bind(&util::default_close_cb, std::placeholders::_1));
        conn_->get_connection()->set_read_callback(std::bind(&util::default_message_cb, std::placeholders::_1, std::placeholders::_2));
        conn_->get_connection()->set_write_complete_callback(std::bind(&util::default_write_complete_cb, std::placeholders::_1, std::placeholders::_2));
    }

    ~tcp_client()
    {
    }

    tcp_client(const tcp_client& rhs) = delete;
    tcp_client& operator=(const tcp_client& rhs) = delete;
    tcp_client(tcp_client&& rhs) = delete;
    tcp_client& operator=(tcp_client&& rhs) = delete;

    void async_connect()
    {
        conn_->async_connect();
    }

    bool sync_connect_wait_for(std::uint32_t sec)
    {
        return conn_->sync_connect_wait_for(sec);
    }

    void set_connection_callback(connection_callback cb)
    {
        conn_->get_connection()->set_connection_callback(std::move(cb));
    }

    void set_message_callback(read_callback cb)
    {
        conn_->get_connection()->set_read_callback(std::move(cb));
    }

    void set_write_complete_callback(write_complete_callback cb)
    {
        conn_->get_connection()->set_write_complete_callback(std::move(cb));
    }

    void set_close_callback(close_callback cb)
    {
        close_cb_ = std::move(cb);
    }

    void set_error_callback(error_callback cb)
    {
        error_cb_ = std::move(cb);
    }

    void set_read_timeout_seconds(std::uint32_t sec)
    {
        conn_->get_connection()->set_read_timeout_duration(sec);
    }

    void set_write_complete_callback(std::uint32_t sec)
    {
        conn_->get_connection()->set_write_timeout_duration(sec);
    }

    tcp_connection_ptr& get_connection()
    {
        return conn_->get_connection();
    }

private:
    void handle_connection_cleanup(const tcp_connection_ptr& conn, const error_code* ec, bool app_cb)
    {
        if (app_cb) {
            try {
                if (ec->value() == boost::asio::error::eof) {
                    close_cb_(conn);
                }
                else {
                    error_cb_(conn, *ec);
                }
            }
            catch (...) {
            }
        }
        loop_->run_in_loop(std::bind(&tcp_client::remove_connection, this, conn));
    }

    void remove_connection(const tcp_connection_ptr& conn)
    {
        const std::string& name = conn->get_name();
        NET_LOG_TRACE << "tcp client remove connection: " << name;
        conn->get_event_loop()->run_in_loop(std::bind(&tcp_connection::connect_destroy, conn));
    }

private:
    event_loop* loop_;
    inet_address addr_;
    //connection_callback      connection_cb_;
    //message_callback         message_cb_;
    //write_complete_callback   write_complete_cb_;
    close_callback           close_cb_;
    error_callback           error_cb_;
    std::unique_ptr<connector> conn_;

};

} // namespace net
} // namespace zysoft



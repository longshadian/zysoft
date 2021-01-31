#ifndef _ZYSOFT_NET_CONNECTOR_H
#define _ZYSOFT_NET_CONNECTOR_H

#include <cstdint>
#include <memory>

#include <zysoft/net/types.h>
#include <zysoft/net/event_loop.h>
#include <zysoft/net/inet_address.h>
#include <zysoft/net/utility.h>
#include <zysoft/net/tcp_connection.h>

namespace zysoft
{
namespace net
{

class connector 
{
public:
    connector(event_loop* loop, const inet_address& addr, std::string name = "")
        : loop_(loop)
        , server_addr_(addr)
        , connection_()
    {
        if (name == "") {
            name = create_connection_name(addr);
        }
        connection_ = tcp_connection::create(loop_, std::move(name));
    }

    ~connector()
    {
    }

    connector(const connector&) = delete;
    connector& operator=(const connector&) = delete;
    connector(connector&&) = delete;
    connector& operator=(connector&&) = delete;

    void async_connect()
    {
        async_connect_impl();
    }

    bool sync_connect_wait_for(std::uint32_t sec)
    {
        // TODO 暂时等待1秒，是否能改成真正同步发送？
        if (sec == 0) {
            sec = 1;
        }
        auto f = sync_connect_wait_for_impl();
        std::future_status fs = f.wait_for(std::chrono::seconds{ sec });
        if (fs == std::future_status::ready) {
            std::int32_t value = f.get();
            if (value == 0)
                return true;
            return false;
        } else {
            // TODO shutdown connection
            return false;
        }
    }

    tcp_connection_ptr& get_connection()
    {
        return connection_;
    }

private:
    void async_connect_impl()
    {
        auto ep = util::create_endpoint(server_addr_.ip_, server_addr_.port_);
        connection_->get_socket().async_connect(ep,
            [this](boost::system::error_code ec)
            {
                if (ec) {
                    retry();
                    return;
                }
                connection_->connect_establised();
            }
        );
    }

    std::future<std::int32_t> sync_connect_wait_for_impl()
    {
        auto p = std::make_shared<std::promise<std::int32_t>>();
        auto ep = util::create_endpoint(server_addr_.ip_, server_addr_.port_);
        connection_->get_socket().async_connect(ep,
            [this, p](const boost::system::error_code& ec)
            {
                if (ec) {
                    p->set_value(ec.value());
                    return;
                }
                p->set_value(0);
                connection_->connect_establised();
            }
        );
        return p->get_future();
    }

    void retry()
    {
        // TODO retry connect
    }

private:
    event_loop*                     loop_;
    inet_address                    server_addr_;
    std::shared_ptr<tcp_connection> connection_;
};


} // namespace net
} // namespace zysoft

#endif // !_ZYSOFT_NET_CONNECTOR_H

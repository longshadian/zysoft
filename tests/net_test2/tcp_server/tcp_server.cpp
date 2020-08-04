#pragma once

#include <iostream>

#include <zysoft/experimental/net.h>

using namespace zysoft;

void Server_ConnectionCB(const net::tcp_connection_ptr& conn)
{
    NET_LOG_INFO << "ConnectionCB new conn: " << conn->get_name();
}

void Server_TimeoutCB(const net::tcp_connection_ptr& conn, net::timeout_type type, std::uint64_t duration)
{
    NET_LOG_INFO << "ConnectionCB timeout cb conn: " << conn->get_name() << " timeout type: " 
        << net::util::to_string(type) << " sec: " << duration;
}

void Server_CloseCB(const net::tcp_connection_ptr& conn)
{
    NET_LOG_INFO << "ConnectionCB close cb conn: " << conn->get_name();
}

void Server_ErrorCB(const net::tcp_connection_ptr& conn, const net::error_code& ec)
{
    NET_LOG_INFO << "ConnectionCB error cb conn: " << conn->get_name() << " error code: " << ec.value() << " reason: " << ec.message();
}

void Server_WriteCB(const net::tcp_connection_ptr& conn, std::size_t len)
{
    NET_LOG_INFO << "ConnectionCB write cb conn: " << conn->get_name() << " len: " << len;
}

void Server_ReadCB(net::tcp_connection_ptr conn, net::buffer* buffer)
{
    //std::string s(buffer->ReadablePtr(), buffer->ReadablePtr() + buffer->ReadableBytes());
    NET_LOG_INFO << "MessageCB size: " << buffer->readable_bytes();
    //buffer->Clear();
    const char* p = net::util::find_CRLF(*buffer);
    if (!p) {
        return;
    }
    std::string s2(buffer->readable_ptr(), p);
    buffer->consume(p + 2 - buffer->readable_ptr());
    conn->send(s2 + "<<\r\n");
    if (s2 == "shutdown") {
        NET_LOG_INFO << "------------------------> shutdown " << conn->get_name();
        for (int i = 0; i != 10; ++i) {
            std::string s = "shutdown ====================> " + std::to_string(i) + "<<\r\n";
            conn->send(s);
        }
        conn->shutdown(true);
        //conn->force_close();
    }
    NET_LOG_INFO << "MessageCB " << s2 << " size: " << buffer->readable_bytes();
}

void Server_ReadCB_Echo(net::tcp_connection_ptr conn, net::buffer* buffer)
{
    NET_LOG_INFO << "MessageCB size: " << buffer->readable_bytes();
    const char* p = net::util::find_CRLF(*buffer);
    if (!p) {
        return;
    }
    std::string s2(buffer->readable_ptr(), p);
    buffer->consume(p + 2 - buffer->readable_ptr());
    conn->send(s2 + "\r\n");
    NET_LOG_INFO << "MessageCB " << s2 << " size: " << buffer->readable_bytes();
}

void TestEcho()
{
    try {
        std::string ip = "0.0.0.0";
        std::uint16_t port = 10086;
        net::inet_address addr{ ip, port };
        net::event_loop loop;
        net::tcp_server server{ &loop, addr };
        server.set_reuse_addr(true);
        server.set_thread_num(2);
        server.set_connection_callback(std::bind(&Server_ConnectionCB, std::placeholders::_1));
        server.set_read_callback(std::bind(&Server_ReadCB_Echo, std::placeholders::_1, std::placeholders::_2));
        server.set_write_complete_callback(std::bind(&Server_WriteCB, std::placeholders::_1, std::placeholders::_2));
        server.set_timeout_callback(std::bind(&Server_TimeoutCB, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        server.set_close_callback(std::bind(&Server_CloseCB, std::placeholders::_1));
        server.set_error_callback(std::bind(&Server_ErrorCB, std::placeholders::_1, std::placeholders::_2));
        server.set_read_timeout(6);
        server.set_write_timeout(10);

        server.start();
        loop.loop();
    }
    catch (const std::exception& e) {
        NET_LOG_CRIT << "TestEcho exception: " << e.what();
    }
}

int main()
{
    TestEcho();
    std::vector<int> v;
    v.push_back(2);
    return 0;
}


#pragma once

#include <iostream>

#include <zysoft/experimental/net.h>

void Server_ConnectionCB(zysoft::net::tcp_connection_ptr conn)
{
    NET_LOG_INFO << "ConnectionCB new conn: " << conn->get_name();
}

void Server_MessageCB(zysoft::net::tcp_connection_ptr conn, zysoft::net::buffer* buffer)
{
    //std::string s(buffer->ReadablePtr(), buffer->ReadablePtr() + buffer->ReadableBytes());
    NET_LOG_INFO << "MessageCB size: " << buffer->readable_bytes();
    //buffer->Clear();
    const char* p = zysoft::net::util::find_CRLF(*buffer);
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

void TestEcho()
{
    try {
        std::string ip = "0.0.0.0";
        std::uint16_t port = 10086;
        zysoft::net::inet_address addr{ ip, port };
        zysoft::net::event_loop loop;
        zysoft::net::tcp_server server{ &loop, addr };
        server.set_reuse_addr(true);
        server.set_thread_num(2);
        server.set_connection_callback(std::bind(&Server_ConnectionCB, std::placeholders::_1));
        server.set_message_callback(std::bind(&Server_MessageCB, std::placeholders::_1, std::placeholders::_2));

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


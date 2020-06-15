#include <iostream>
#include <cstdio>
#include <cstdint>
#include <chrono>
#include <thread>

#include <zysoft/experimental/net.h>

void Client_ConnectCB(const zysoft::net::tcp_connection_ptr& conn)
{
    NET_LOG_INFO << "client connect cb: " << conn->get_name();
}

void Client_ClosedCB(const zysoft::net::tcp_connection_ptr& conn)
{
    NET_LOG_INFO << "client closedcb: " << conn->get_name();
}

void Client_MessageCB(const zysoft::net::tcp_connection_ptr& conn, zysoft::net::buffer* buffer)
{
    NET_LOG_INFO << "client messagecb: " << conn->get_name() << " " << buffer->readable_bytes();
    const char* p = zysoft::net::util::find_CRLF(*buffer);
    if (!p) {
        return;
    }
    std::string s(buffer->readable_ptr(), p);
    buffer->consume(p + 2 - buffer->readable_ptr());
    NET_LOG_INFO << conn->get_name() << " received bytes: " << (s.size() + 2) << " content: " << s;
}

void Client_WriteCB(const zysoft::net::tcp_connection_ptr& conn, std::size_t len)
{
    NET_LOG_INFO << "client writecb: " << conn->get_name() << " send bytes: " << len;
}

struct EventLoopPool
{
public:
    EventLoopPool()
        : pool_(nullptr)
    {
    }

    ~EventLoopPool()
    {
    }

    void Start(int n = 1)
    {
        if (n <= 0) {
            n = 1;
        }
        pool_.thread_num(n);
        pool_.sync_start();
    }

    zysoft::net::event_loop* NextLoop()
    {
        return pool_.next_event_loop();
    }
private:
    zysoft::net::event_loop_thread_pool pool_;
};

void Test1()
{
    EventLoopPool pool;
    pool.Start();
    // 普通ping测试
    const std::string host = "127.0.0.1";
    std::uint16_t port = 10086;
    zysoft::net::inet_address server_addr(host, port);
    zysoft::net::tcp_client client(pool.NextLoop(), server_addr);

    client.set_connection_callback(std::bind(&::Client_ConnectCB, std::placeholders::_1));
    client.set_message_callback(std::bind(&::Client_MessageCB, std::placeholders::_1, std::placeholders::_2));
    client.set_write_complete_callback(std::bind(&::Client_WriteCB, std::placeholders::_1, std::placeholders::_2));

    assert(client.sync_connect_wait_for(10));

    int n = 0;
    while (1) {
        ++n;
        if (n < 10)
            client.get_connection()->send("aaaaaaaaaaaaaa " + std::to_string(n) + "\r\n");
        std::this_thread::sleep_for(std::chrono::seconds{ 1 });
    }
}

#if 0
void Test1()
{
    // 普通ping测试
    const std::string host = "127.0.0.1";
    std::uint16_t port = 8080;
    auto g_client = StartClient(2);

    auto conn = g_client->CreateConnector();
    if (0) {
        g_client->AsyncConnect(conn, host, port);
    } else {
        g_client->SyncConnect(conn, host, port);
    }

    int n = 0;
    while (1) {
        ++n;
        if (conn->Connected()) {
            conn->GetChannel()->SendMsg("aaaaaaaaaaaaaa " + std::to_string(n));
        } else {
            conn = g_client->CreateConnector();
            auto succ = g_client->SyncConnect(conn, host, port);
            if (succ) {
                conn->GetChannel()->SendMsg("aaaaaaaaaaaaaa " + std::to_string(n));
            } else {
                WPrintf("connect failed.");
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds{ 1 });
    }
}
#endif

#if 0
void Test2()
{
    // 测试多客户端同时发送
    const std::string host = "127.0.0.1";
    std::uint16_t port = 8080;
    auto p_client = StartClient(2);

    std::vector<network::TcpConnectorPtr> vec{};
    for (int i = 0; i != 10; ++i) {
        auto conn = p_client->CreateConnector();
        p_client->SyncConnect(conn, host, port);
        if (!conn->Connected()) {
            WPrintf("connect failed.");
            return;
        }
        vec.push_back(conn);
    }

    std::string s{};
    s.resize(1024, 'a');
    int n = 0;
    std::thread temp([&n]()
        {
            while (1) {
                DPrintf("n: %d", n);
                std::this_thread::sleep_for(std::chrono::seconds{ 1 });
            }
        });

    temp.detach();

    while (1) {
        for (auto& conn : vec) {
            if (conn->Connected()) {
                ++n;
                conn->GetChannel()->SendMsg(s + std::to_string(n));
            } else {
                WPrintf("connect shutdown");
            }
        }
        //std::this_thread::sleep_for(std::chrono::microseconds{ 1 });
    }
}
#endif

int main()
{
    try {
        Test1();
    } catch (const std::exception& e) {
        NET_LOG_CRIT << "exception: " << e.what();
    }
    return 0;
}


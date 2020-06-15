#pragma once

#include <cstdint>

#include <zysoft/experimental/net/types.h>
#include <zysoft/experimental/net/tcp_connection.h>
#include <zysoft/experimental/net/inet_address.h>
#include <zysoft/experimental/net/console_log.h>

namespace zysoft
{
namespace net
{
namespace util
{


inline
const char* null_string()
{
    static const char c = '\0';
    return &c;
}

inline
boost::asio::ip::tcp::endpoint create_endpoint(const std::string& host, std::uint16_t port)
{
    auto addr = boost::asio::ip::address_v4::from_string(host);
    return boost::asio::ip::tcp::endpoint{ addr, port };
}

template <typename T>
inline
void bzero(T* t)
{
    //static_assert(std::is_pod<T>::value, "T must be POD!");
    std::memset(t, 0, sizeof(T));
}

inline
std::uint64_t NextIndex()
{
    static std::atomic<std::uint64_t> idx_ = { 0 };
    return ++idx_;
}

inline
const char* find(const char* b, std::size_t b_len, const char* p, std::size_t p_len)
{
    const char* b_e = b + b_len;
    const char* it = std::search(b, b_e, p, p + p_len);
    if (it == b_e)
        return nullptr;
    return it;
}

inline
const char* find(const buffer& buffer, const char* p, std::size_t len)
{
    return find(buffer.readable_ptr(), buffer.readable_bytes(), p, len);
}

inline
const char* find_CRLF(const buffer& b)
{
    static const char* p = "\r\n";
    return find(b, p, 2);
}

inline
const char* find_EOL(const buffer& b)
{
    static const char* p = "\n";
    return find(b, p, 1);
}

inline
void default_connection_cb(const tcp_connection_ptr& conn)
{
    NET_LOG_TRACE << "Default connection callback. " << conn->get_name();
}

inline
void default_message_cb(const tcp_connection_ptr& conn, buffer* b)
{
    NET_LOG_TRACE << "Default message callback. " << conn->get_name() << " buffer bytes: " << b->readable_bytes();
    b->clear();
}

inline
void default_write_complete_cb(const tcp_connection_ptr& conn, std::size_t len)
{
    NET_LOG_TRACE << "Default write complete callback. " << conn->get_name() << " write bytes: " << len;
}

inline
void default_close_cb(const tcp_connection_ptr& conn)
{
    NET_LOG_TRACE << "Default close callback. " << conn->get_name();
}

inline
void default_error_cb(const tcp_connection_ptr& conn, const error_code& ecode)
{
    NET_LOG_TRACE << "Default error callback. " << conn->get_name() << " error code: " << ecode.value() << " reason: " << ecode.message();
}

inline
void default_timeout_cb(const tcp_connection_ptr& conn, timeout_type type, std::uint64_t sec)
{
    NET_LOG_TRACE << "Default timeout callback. " << conn->get_name() << " timeout type: " 
        << (type == timeout_type::read ? "read" : "write")
        << " seconds: " << sec;
}

inline
std::string create_connection_name(const inet_address& addr, int conn_id)
{
    char buffer[64] = {0};
    snprintf(buffer, sizeof buffer, "%s:%d#%d", addr.ip_.c_str(), static_cast<int>(addr.port_), conn_id);
    return buffer;
}

inline
std::string create_connection_name(const inet_address& addr)
{
    char buffer[64] = {0};
    snprintf(buffer, sizeof buffer, "%s:%d", addr.ip_.c_str(), static_cast<int>(addr.port_));
    return buffer;
}

inline
const std::string& to_string(timeout_type t)
{
    static const std::string str_read = "read";
    static const std::string str_write = "write";
    return t == timeout_type::read ? str_read : str_write;
}


} // namespace util
} // namespace net
} // namespace zysoft

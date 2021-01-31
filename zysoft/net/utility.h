#ifndef _ZYSOFT_NET_UTILITY_H
#define _ZYSOFT_NET_UTILITY_H

#include <cstdint>

#include <zysoft/net/types.h>
#include <zysoft/net/buffer.h>
        
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

/*
void default_connection_cb(const tcp_connection_ptr& conn);
void default_message_cb(const tcp_connection_ptr& conn, buffer* b);
void default_write_complete_cb(const tcp_connection_ptr& conn, std::size_t len);
void default_close_cb(const tcp_connection_ptr& conn);
void default_error_cb(const tcp_connection_ptr& conn, const error_code& ecode);
void default_timeout_cb(const tcp_connection_ptr& conn, timeout_type type, std::uint64_t sec);
*/

/*
std::string create_connection_name(const inet_address& addr, int conn_id);
std::string create_connection_name(const inet_address& addr);
*/

inline
const std::string& to_string(timeout_type t)
{
    static const std::string str_read = "read";
    static const std::string str_write = "write";
    return t == timeout_type::read ? str_read : str_write;
}

template <typename F, typename... Args>
inline
void invoke_no_except(F& f, Args&&... arg)
{
    try {
        f(std::forward<Args>(arg)...);
    } catch (...) {
    }
}

} // namespace util
} // namespace net
} // namespace zysoft

#endif // !_ZYSOFT_NET_UTILITY_H

#ifndef _ZYSOFT_NET_TYPES_H
#define _ZYSOFT_NET_TYPES_H

#include <functional>
#include <memory>
#include <cstdint>

#include <boost/asio.hpp>
#include <boost/utility/string_view.hpp>

namespace zysoft
{
namespace net
{

enum class timeout_type
{
    read = 0,
    write = 1,
};

class buffer;
class tcp_connection;
using tcp_connection_ptr = std::shared_ptr<tcp_connection>;
using error_code = boost::system::error_code;

using connection_callback = std::function<void(const tcp_connection_ptr&)>;
using read_callback = std::function<void(const tcp_connection_ptr&, buffer*)>;
using write_complete_callback = std::function<void(const tcp_connection_ptr&, std::size_t len)>;
using close_callback = std::function<void(const tcp_connection_ptr&)>;
using error_callback = std::function<void(const tcp_connection_ptr&, const error_code& ecode)>;
using timeout_callback = std::function<void(const tcp_connection_ptr&, timeout_type, std::uint64_t sec)>;

int constexpr default_read_timeout_seconds() { return 60; }
int constexpr default_write_timeout_seconds() { return 60; }

namespace detail 
{

enum class cleanup_type
{
    socket_err = 0,
    timeout = 1,
    activity = 2,
};

struct cleanup_context 
{
    cleanup_context()
        : type_(cleanup_type::socket_err)
        , call_error_cb_(false)
        , err_code_()
        , timeout_type_(timeout_type::read)
        , timeout_sec_(0)
    {
    }

    cleanup_type type_;
    bool call_error_cb_;
    error_code err_code_;

    timeout_type timeout_type_;
    std::uint64_t timeout_sec_;
};

using conn_cleanup_callback = std::function<void(const tcp_connection_ptr&, const cleanup_context& ctx)>;

} // namespace detail

} // namespace net
} // namespace zysoft


#endif // !_ZYSOFT_NET_TYPES_H

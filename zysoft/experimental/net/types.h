#pragma once

#include <functional>
#include <memory>

#include <boost/asio.hpp>

namespace zysoft
{
namespace net
{

class buffer;
class tcp_connection;
using tcp_connection_ptr = std::shared_ptr<tcp_connection>;
using err_code = boost::system::error_code;

using connection_callback = std::function<void(const tcp_connection_ptr&)>;
using message_callback = std::function<void(const tcp_connection_ptr&, buffer*)>;
using write_complete_callback = std::function<void(const tcp_connection_ptr&)>;
using close_callback = std::function<void(const tcp_connection_ptr&)>;
using error_callback = std::function<void(const tcp_connection_ptr&, const err_code& ecode)>;


namespace detail 
{

using conn_cleanup_callback = std::function<void(const tcp_connection_ptr&, const err_code*, bool)>;

} // namespace detail

} // namespace net
} // namespace zysoft



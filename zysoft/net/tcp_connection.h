#ifndef _ZYSOFT_NET_TCP_CONNECTION_H
#define _ZYSOFT_NET_TCP_CONNECTION_H

#include <array>
#include <list>
#include <string>

#include <zysoft/net/types.h>
#include <zysoft/net/buffer.h>
#include <zysoft/net/event_loop.h>
#include <zysoft/net/utility.h>

namespace zysoft
{
namespace net
{

class tcp_connection 
    : public std::enable_shared_from_this<tcp_connection>
{
public:
    using socket_type = boost::asio::ip::tcp::socket;

    static const std::size_t FixedBufferSize = 4 * 1024;
    enum class EState
    {
        // tcp_connection对象刚创建，或者已经destroy
        Closed,

        // 连接已经建立，还未注册async_read事件
        Connecting,

        // 连接已经建立，已注册async_read事件，可正常发送数据
        Established,

        // 有以下三种情况会导致tcp_connection对象处于WaitClose状态
        // 1. socket可正常读写，但是即将关闭，此时应用无法发送数据。但是连接可正常接收/发送数据。
        // 例如调用shutdown函数后，如果out_buffer中还有数据未发送，会在这些数据发送后，执行cleanup
        // 2. socket读写发生错误，执行cleanup
        // 3. socket读写timeout，执行cleanup
        // 4. 执行force_close，执行cleanup
        WaitClose,
    };

    enum EFlag
    {
        OptShutdown                 = 0x0001,
        OptForeceClose              = 0x0002,

        OptDoNotMessageCallback     = 0x0004,
    };

private:
    tcp_connection(event_loop* loop, std::string conn_name)
        : loop_(loop)
        , conn_name_(std::move(conn_name))
        , socket_(loop_->get_io_context().get_executor())
        , state_(EState::Closed)
        , flag_()
        , input_fixed_buffer_()
        , input_buffer_()
        , output_buffer_()
        , is_cleaned_(false)
        , connection_cb_()
        , message_cb_()
        , write_complete_cb_()
        , cleanup_cb_()
        , read_timer_()
        , write_timer_()
        , read_timeout_sec_(0)
        , write_timeout_sec_(0)
    {
    }

public:
    ~tcp_connection()
    {
        // 析构函数中不调用cleanup，只执行destroy
        // 1. cleanup可能会执行回调，回调对象有可能已经失效
        // 2. destroy只关闭自身持有的socket，此操作是安全的
        connect_destroy();
    }

    tcp_connection(const tcp_connection&) = delete;
    tcp_connection& operator=(const tcp_connection&) = delete;
    tcp_connection(tcp_connection&&) = delete;
    tcp_connection& operator=(tcp_connection&&) = delete;

    static
    tcp_connection_ptr create(event_loop* loop, std::string conn_name)
    {
        auto* pthis = new tcp_connection(loop, std::move(conn_name));
        return tcp_connection_ptr(pthis);
    }

    void set_connection_callback(connection_callback cb) { connection_cb_ = std::move(cb); } 
    void set_read_callback(read_callback cb) { message_cb_ = std::move(cb); }
    void set_write_complete_callback(write_complete_callback cb) { write_complete_cb_ = std::move(cb); }
    void set_cleanup_callback(detail::conn_cleanup_callback cb) { cleanup_cb_ = std::move(cb); }
    void set_read_timeout_duration(int sec) { read_timeout_sec_ = sec; }
    void set_write_timeout_duration(int sec) { write_timeout_sec_ = sec; }

    const std::string& get_name() const { return conn_name_; }
    event_loop* get_event_loop() { return loop_; }
    socket_type& get_socket() { return socket_; }

    // TODO private
    void set_state(EState s)
    {
        state_ = s;
    }

    void connect_establised()
    {
        state_ = EState::Established;
        assert(!read_timer_);
        if (read_timeout_sec_ > 0) {
            read_timer_ = get_event_loop()->get_timer_queue()->add_timer(
                std::bind(&tcp_connection::handle_timeout, this, timeout_type::read, std::placeholders::_1),
                read_timeout_sec_
            );
        }
        async_read();
        util::invoke_no_except(connection_cb_, shared_from_this());
    }

    void connect_destroy()
    {
        if (state_ == EState::Closed)
            return;
        state_ = EState::Closed;
        error_code ec{};
        socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);
        ec.clear();
        socket_.close(ec);
    }

    void send(const std::string& str) { send(str.data(), str.size()); }
    void send(const void* p, std::size_t len)
    {
        buffer b;
        b.append(p, len);
        send(std::move(b));
    }
    void send(const buffer& message) { send_impl(buffer(message)); }
    void send(buffer&& message) { send_impl(std::move(message)); }

    void shutdown(bool callback = false)
    {
        if (flag_ & EFlag::OptShutdown)
            return;
        if (flag_ & EFlag::OptForeceClose)
            return;
        flag_ |= EFlag::OptShutdown;
        if (!callback)
            flag_ |= EFlag::OptDoNotMessageCallback;

#if 0
        // 不能直接执行socket cancel，需要异步执行，理由如下：
        // 当前可能是HandleRead事件回调，此时如果未注册Write事件，cancel不会触发HandelError
        loop_->run_in_loop( 
            [this]() mutable 
            { 
                shutdown_in_loop(); 
            } 
        );
#else
        if (loop_->is_in_loop_thread()) {
            shutdown_in_loop();
        } else {
            loop_->run_in_loop( 
                [this]() mutable 

                { 
                    shutdown_in_loop(); 
                } 
            );
        }
#endif
    }

    void force_close() 
    {
        if (flag_ & EFlag::OptForeceClose)
            return;
        flag_ |= EFlag::OptForeceClose;
        flag_ |= EFlag::OptDoNotMessageCallback;

#if 0
        // 不能直接执行socket cancel，需要异步执行，理由如下：
        // 当前可能是HandleRead事件回调，此时如果未注册Write事件，cancel不会触发HandelError
        loop_->run_in_loop( 
            [this]() mutable 
            { 
                force_close_in_loop();
            } 
        );
#else
        if (loop_->is_in_loop_thread()) {
            force_close_in_loop();
        } else {
            loop_->run_in_loop( 
                [this]() mutable 
                { 
                    force_close_in_loop();
                } 
            );
        }
#endif
    }

    void assert_run_in_loop()
    {
        assert(loop_->is_in_loop_thread());
    }

private:
    void async_read()
    {
        if (read_timer_)
            get_event_loop()->get_timer_queue()->reset_timer(read_timer_);
        socket_.async_read_some(boost::asio::buffer(input_fixed_buffer_),
            std::bind(&tcp_connection::handle_read, this, std::placeholders::_1, std::placeholders::_2));
    }

    void async_write()
    {
        if (write_timeout_sec_ > 0) {
            if (write_timer_) {
                get_event_loop()->get_timer_queue()->reset_timer(write_timer_);
            } else {
                write_timer_ = get_event_loop()->get_timer_queue()->add_timer(
                    std::bind(&tcp_connection::handle_timeout, this, timeout_type::write, std::placeholders::_1),
                    write_timeout_sec_
                );
            }
        }

        const auto& buffer = output_buffer_.front();
        boost::asio::async_write(socket_, boost::asio::buffer(buffer.readable_ptr(), buffer.readable_bytes()),
            std::bind(&tcp_connection::handle_write, this, std::placeholders::_1, std::placeholders::_2));
    }

    void handle_read(error_code ec, std::size_t len)
    {
        assert_run_in_loop();
        if (ec) {
            handle_read_write_error(ec);
            return;
        }
        ZYSOFT_NET_TRACE("%s read bytes: %lld", get_name().c_str(), static_cast<long long>(len));
        input_buffer_.append(input_fixed_buffer_.data(), len);
        if (need_message_callback()) {
            util::invoke_no_except(message_cb_, shared_from_this(), &input_buffer_);
        } else {
            // 应用主动调用shutdown，此时socket可能还处在正常连接状态，
            // 会接收到对端发送的数据，直接丢弃，不执行应用程序回调。
            input_buffer_.clear();
        }

        async_read();
    }

    void handle_write(error_code ec, std::size_t len)
    {
        assert_run_in_loop();
        buffer msg = std::move(output_buffer_.front());
        output_buffer_.pop_front();
        (void)msg;
        if (ec) {
            handle_read_write_error(ec);
            return;
        }
        if (write_complete_cb_) {
            util::invoke_no_except(write_complete_cb_, shared_from_this(), len);
        }

        if (state_ == EState::WaitClose) {
            if (flag_ & EFlag::OptForeceClose) {
                cancel_timer(write_timer_);
                return;
            }
            if (flag_ & EFlag::OptShutdown) {
                if (output_buffer_.empty()) {
                    // 当前无消息可发送，取消异步操作，HandleRead会被执行。
                    detail::cleanup_context ctx;
                    ctx.type_ = detail::cleanup_type::activity;
                    ctx.err_code_ = boost::asio::error::make_error_code(boost::asio::error::eof);
                    connect_cleanup(ctx);
                } else {
                    // 还有消息待发送，继续发送。
                    async_write();
                }
            }
            return;
        }

        if (!output_buffer_.empty()) {
            async_write();
        } else {
            cancel_timer(write_timer_);
        }
    }

    void handle_read_write_error(const error_code& ec)
    {
        // 此函数有2种情况会被调用:
        // 1.应用主动调用Shutdown或ForceClose，触发read/write取消操作。
        //     此时，socket状态已经处于Closed状态
        // 2.read/write操作返回错误，例如对端主动关闭socket，或者socket read，write出错。
        ZYSOFT_NET_TRACE("Handle Close: %s ecode: %d emsg: %s", get_name().c_str(), ec.value(), ec.message());
        detail::cleanup_context ctx;
        ctx.type_ = detail::cleanup_type::socket_err;
        ctx.err_code_ = ec;
        connect_cleanup(ctx);
    }

    void handle_timeout(timeout_type type, std::uint64_t sec)
    {
        assert_run_in_loop();
        if (state_ == EState::Closed) {
            ZYSOFT_NET_TRACE("conn: %s timeout:  state is closed", get_name().c_str(), static_cast<int>(type));
            return;
        }

        // TODO callback
        if (type == timeout_type::read) {
            assert(read_timer_);
            ZYSOFT_NET_TRACE("read timeout %s seconds: %d", get_name().c_str(), sec);
        } else {
            assert(write_timer_);
            ZYSOFT_NET_TRACE("write timeout %s seconds: %d", get_name().c_str(), sec);
        }
        detail::cleanup_context ctx;
        ctx.type_ = detail::cleanup_type::timeout;
        ctx.timeout_type_ = type;
        ctx.timeout_sec_ = sec;

        // 超时后，执行清理操作
        connect_cleanup(ctx);
    }

    void send_impl(buffer&& message)
    {
        if (loop_->is_in_loop_thread()) {
            send_in_loop(std::move(message));
        } else {
            loop_->run_in_loop(
                [this, msg = std::move(message)]() mutable 
                { 
                    send_in_loop(std::move(msg)); 
                }
            );
        }
    }

    void send_in_loop(buffer&& msg)
    {
        if (state_ == EState::WaitClose || state_ == EState::Closed) {
            return;
        }
        bool wait_write = !output_buffer_.empty();
        output_buffer_.emplace_back(std::move(msg));
        if (!wait_write) {
            async_write();
        }
    }

    void shutdown_in_loop()
    {
        if (state_ == EState::Closed || state_ == EState::WaitClose) {
            return;
        }
        state_ = EState::WaitClose;
        if (output_buffer_.empty()) {
            // 当前无消息待发送，执行清理操作
            detail::cleanup_context ctx;
            ctx.type_ = detail::cleanup_type::activity;
            ctx.err_code_ = boost::asio::error::make_error_code(boost::asio::error::eof);
            connect_cleanup(ctx);
        }
    }

    void force_close_in_loop()
    {
        if (state_ == EState::Closed) {
            return;
        }
        // 执行清理操作
        detail::cleanup_context ctx;
        ctx.type_ = detail::cleanup_type::activity;
        ctx.err_code_ = boost::asio::error::make_error_code(boost::asio::error::eof);
        connect_cleanup(ctx);
    }

    bool need_message_callback() const
    {
        return ! (flag_ & EFlag::OptDoNotMessageCallback);
    }

    void cancel_timer(timer_queue::timer_handle_ptr& ptr)
    {
        if (ptr) {
            get_event_loop()->get_timer_queue()->cancel_timer(ptr);
            ptr = nullptr;
        }
    }

    void connect_cleanup(detail::cleanup_context& ctx)
    {
        if (state_ == EState::Closed)
            return;
        if (state_ != EState::WaitClose) {
            state_ = EState::WaitClose;
        }
        // 确保只此函数只会被调用一次。防止cleanup执行多次。
        if (is_cleaned_)
            return;
        is_cleaned_ = true;

        cancel_timer(read_timer_);
        cancel_timer(write_timer_);

        // 取消异步操作，handle_read_write_error(如果存在)会被调用。
        error_code ec{};
        socket_.cancel(ec);
        ctx.call_error_cb_ = need_message_callback();
        cleanup_cb_(shared_from_this(), ctx);
    }

private:
    event_loop*                 loop_;
    std::string                 conn_name_;
    socket_type                 socket_;
    EState                      state_;
    int                         flag_;
    std::array<char, FixedBufferSize> input_fixed_buffer_;
    buffer                      input_buffer_;
    std::list<buffer>           output_buffer_;
    bool                        is_cleaned_;

    connection_callback             connection_cb_;
    read_callback                message_cb_;
    write_complete_callback         write_complete_cb_;
    detail::conn_cleanup_callback   cleanup_cb_;

    timer_queue::timer_handle_ptr   read_timer_;
    timer_queue::timer_handle_ptr   write_timer_;
    int                             read_timeout_sec_;
    int                             write_timeout_sec_;
};

inline
void default_connection_cb(const tcp_connection_ptr& conn)
{
    ZYSOFT_NET_TRACE("Default connection callback: ", conn->get_name().c_str());
}

inline
void default_message_cb(const tcp_connection_ptr& conn, buffer* b)
{
    ZYSOFT_NET_TRACE("Default message callback: %s buffer bytes: ", conn->get_name().c_str(), b->readable_bytes());
    b->clear();
}

inline
void default_write_complete_cb(const tcp_connection_ptr& conn, std::size_t len)
{
    ZYSOFT_NET_TRACE("Default write complete callback: %s write bytes: ", conn->get_name().c_str(),len);
}

inline
void default_close_cb(const tcp_connection_ptr& conn)
{
    ZYSOFT_NET_TRACE("Default close callback: %s ", conn->get_name().c_str());
}

inline
void default_error_cb(const tcp_connection_ptr& conn, const error_code& ecode)
{
    ZYSOFT_NET_TRACE("Default error callback: %s ecode: %d emsg: %s", conn->get_name().c_str(), ecode.value(), ecode.message());
}

inline
void default_timeout_cb(const tcp_connection_ptr& conn, timeout_type type, std::uint64_t sec)
{
    ZYSOFT_NET_TRACE("Default timeout callback: %s timeout type: %s  second: %d", conn->get_name().c_str(), (type == timeout_type::read ? "read" : "write")
        , sec);
}

} // namespace net
} // namespace zysoft

#endif // !_ZYSOFT_NET_TCP_CONNECTION_H


#ifndef _ZYSOFT_NET_BUFFER_H
#define _ZYSOFT_NET_BUFFER_H

#include <algorithm>
#include <cassert>
#include <string_view>
#include <string>
#include <vector>
    
namespace zysoft
{
namespace net
{

class buffer 
{
public:
    static const std::size_t PrependBytes = 8;
    static const std::size_t InitSize = 4 * 1024;

public:
    buffer(std::size_t n = InitSize)
        : buffer_(PrependBytes + n)
        , read_pos_(PrependBytes)
        , write_pos_(PrependBytes)
    {
        assert(readable_bytes() == 0);
        assert(writable_bytes() == n);
    }

    ~buffer() = default;
    buffer(const buffer& rhs) = default;
    buffer& operator=(const buffer& rhs) = default;
    buffer(buffer&& rhs) = default;
    buffer& operator=(buffer&& rhs) = default;

    bool empty() const
    {
        return read_pos_ == write_pos_;
    }

    void clear()
    {
        read_pos_ = PrependBytes;
        write_pos_ = PrependBytes;
    }

    std::size_t readable_bytes() const
    {
        return write_pos_ - read_pos_;
    }

    std::size_t writable_bytes() const
    {
        return buffer_.size() - write_pos_;
    }

    const char* readable_ptr() const
    {
        return data() + read_pos_;
    }

    void consume(std::size_t len)
    {
        assert(len <= readable_bytes());
        if (len < readable_bytes()) {
            read_pos_ += len;
        } else {
            clear();
        }
    }

    void append(const std::string& s)
    {
        append(s.data(), s.size());
    }

    void append(const void* d, std::size_t len)
    {
        if (!d || len == 0)
            return;
        prepare_writable_bytes(len);
        const char* p = static_cast<const char*>(d);
        std::copy(p, p + len, data() + write_pos_);
        write_pos_ += len;
    }

    void shrink()
    {
        std::size_t readable = readable_bytes();
        std::copy(data() + read_pos_, data() + write_pos_, data() + PrependBytes);
        read_pos_ = PrependBytes;
        write_pos_ = read_pos_ + readable;
        buffer_.resize(write_pos_);
        buffer_.shrink_to_fit();
    }

    void swap(buffer& rhs)
    {
        std::swap(buffer_, rhs.buffer_);
        std::swap(read_pos_, rhs.read_pos_);
        std::swap(write_pos_, rhs.write_pos_);
    }

private:
    char* data()
    {
        return buffer_.data();
    }

    const char* data() const
    {
        return buffer_.data();
    }

    void make_space(std::size_t len)
    {
        if (read_pos_ - PrependBytes + writable_bytes() < len) {
            buffer_.resize(write_pos_ + len);
        } else {
            std::size_t readable = readable_bytes();
            std::copy(data() + read_pos_, data() + write_pos_, data() + PrependBytes);
            read_pos_ = PrependBytes;
            write_pos_ = read_pos_ + readable;
        }
    }

    void prepare_writable_bytes(std::size_t len)
    {
        if (writable_bytes() < len) {
            make_space(len);
        }
        assert(writable_bytes() >= len);
    }

private:
    std::vector<char> buffer_;
    std::size_t read_pos_;
    std::size_t write_pos_;
};

} // namespace net
} // namespace zysoft

#endif // !_ZYSOFT_NET_BUFFER_H


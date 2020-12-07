#ifndef _ZYSOFT_ZYSOFT_MEMORY_FALT_BUFFER_H
#define _ZYSOFT_ZYSOFT_MEMORY_FALT_BUFFER_H

#include <algorithm>
#include <cassert>
#include <vector>
#include <string>
#include <string_view>
    
namespace zysoft
{

template< std::size_t PSize
        , std::size_t ISize
        >
class basic_flat_buffer
{
public:
    static const std::size_t PrependBytes = PSize;
    static const std::size_t InitSize = ISize;

public:
    explicit basic_flat_buffer(std::size_t n = InitSize)
        : buffer_(PrependBytes + n)
        , read_pos_(PrependBytes)
        , write_pos_(PrependBytes)
    {
        assert(readable_bytes() == 0);
        assert(writable_bytes() == n);
    }

    ~basic_flat_buffer() = default;
    basic_flat_buffer(const basic_flat_buffer& rhs) = default;
    basic_flat_buffer& operator=(const basic_flat_buffer& rhs) = default;
    basic_flat_buffer(basic_flat_buffer&& rhs) = default;
    basic_flat_buffer& operator=(basic_flat_buffer&& rhs) = default;

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

    std::size_t consume(std::size_t len)
    {
        std::size_t readable = readable_bytes();
        if (len < readable) {
            read_pos_ += len;
            return len;
        } else {
            clear();
            return readable;
        }
    }

    void append(const std::string& s)
    {
        append(s.data(), s.size());
    }

#if 0
    void append(std::string_view& s)
    {
        append(s.data(), s.size());
    }
#endif

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

    void swap(basic_flat_buffer& rhs)
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

using flat_buffer = basic_flat_buffer<8, 1024>;

} // namespace zysoft

#endif // !_ZYSOFT_ZYSOFT_MEMORY_FALT_BUFFER_H


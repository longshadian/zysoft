#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>

#include <zysoft/zysoft/zysoft.h>
#include <zysoft/zysoft/memory/util/allocator_selector.h>

namespace zysoft
{

template< typename T
        , std::size_t N = 256
        , typename A = typename allocator_selector<T>::allocator_type
        >
class auto_buffer
    : protected A
{
    typedef typename allocator_selector<A>::allocator_traits_type alloc_traits;

    enum {space = N};
public:
    typedef auto_buffer<T, N, A>                    class_type;
    typedef typename alloc_traits::allocator_type   allocator_type;
    typedef typename alloc_traits::value_type       value_type;
    typedef typename alloc_traits::size_type        size_type;
    typedef typename alloc_traits::pointer          pointer;
    typedef typename alloc_traits::const_pointer    const_pointer;

    typedef T&                                      reference;
    typedef const T&                                const_reference;
    typedef T*                                      iterator;
    typedef const iterator                          const_iterator;
    typedef std::reverse_iterator<T>                reverse_iterator;
    typedef const reverse_iterator                  const_reverse_iterator;

    static_assert(std::is_pod_v<T>, "auto_buffer T must be POD!");
    static_assert(N != 0, "auto_buffer N != 0");
public:

    explicit    auto_buffer(size_type n);
                ~auto_buffer();

                auto_buffer(const class_type&) = delete;
                auto_buffer(class_type&&) = delete;
    class_type& operator=(const class_type&) = delete;
    class_type& operator=(class_type&&) = delete;

public:
    bool resize(size_type n)
    {
        if (n == 0) {
            if (external_buffer()) {
                dealloc(buffer_, items_);
                external_ = false;
                buffer_ = &internal_[0];
            }
            items_ = n;
            return true;
        }

        if (items_ < n) {
            if (external_buffer()) {
                pointer new_buffer = realloc(buffer_, items_, n);
                if (!new_buffer)
                    return false;
                buffer_ = new_buffer;
            } else {
                if (space < n) {
                    pointer new_buffer = alloc(n);
                    if (!new_buffer)
                        return false;
                    block_copy(new_buffer, buffer_, items_);
                    buffer_ = new_buffer;
                    external_ = true;
                } else {
                    // Nothing to do
                }
            }
        } else {
            if (external_buffer()) {
                // Nothing to do
            } else {
                // Nothing to do
            }
        }
        items_ = n;
        return true;
    }

    void swap(class_type& rhs)
    {
        std::swap(this->internal_, rhs.m_internal);
        std::swap(this->buffer_, rhs.m_buffer);
        std::swap(this->items_, rhs.m_items);
        std::swap(this->external_, rhs.m_external);
    }

    bool empty() const
    {
        return items_ == 0;
    }

    size_type size() const
    {
        return items_;
    }

    static size_type internal_size()
    {
        return space;
    }

    reference operator[](size_type index)
    {
        ZYSOFT_ASSERT(index < items_);
        return buffer_[index];
    }

    const_reference operator[](size_type index) const
    {
        ZYSOFT_ASSERT(index < items_);
        return buffer_[index];
    }

    pointer data()
    {
        return buffer_;
    }

    const_pointer data() const
    {
        return buffer_;
    }

    iterator begin()
    {
        return buffer_;
    }

    iterator end()
    {
        return buffer_ + items_;
    }

    const_iterator begin() const
    {
        return buffer_;
    }

    const_iterator end() const
    {
        return buffer_ + items_;
    }

    const_iterator cbegin() const
    {
        return buffer_ + items_;
    }

    const_iterator cend() const
    {
        return buffer_ + items_;
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }

    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(begin());
    }

private:
    value_type              internal_[N];
    pointer                 buffer_;
    size_type               items_;
    bool                    external_;

private:
    pointer alloc(size_type n)
    {
        return alloc_traits::allocate(get_allocator(), n);
        //return get_allocator().allocate(n);
    }

    void dealloc(pointer p, size_type n)
    {
        return alloc_traits::deallocate(get_allocator(), p, n);
        //get_allocator().deallocate(p, n);
    }

    pointer realloc(pointer current_p, size_type current_n, size_type new_n)
    {
        pointer new_p = alloc(new_n);
        if (!new_p)
            return new_p;
        block_copy(new_p, current_p, current_n);
        dealloc(current_p, current_n);
        return new_p;
    }

    allocator_type& get_allocator()
    {
        return *this;
    }

    bool external_buffer() const
    {
        return external_;
    }

    static void block_copy(void* s, const void* d, size_type len)
    {
        std::memcpy(s, d, len * sizeof(T));
    }

    static void block_fill(void* s, size_type len)
    {
        std::memset(s, 0, len * sizeof(T));
    }
};


template< typename T
        , std::size_t N
        , typename A
        >
inline auto_buffer<T, N, A>::auto_buffer(size_type n)
    : internal_()
    , buffer_(n < N ? &internal_[0] : alloc(n))
    , items_(buffer_ ? n : 0)
    , external_(n >= N)
{
    block_fill(buffer_, items_);
}

template< typename T
        , std::size_t N
        , typename A
        >
inline auto_buffer<T, N, A>::~auto_buffer()
{
    if (external_buffer()) {
        dealloc(buffer_, items_);
    }
}

} // namespace zysoft


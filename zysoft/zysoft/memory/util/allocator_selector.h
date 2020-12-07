#ifndef _ZYSOFT_ZYSOFT_MEMORY_UTIL_ALLOCATOR_SELECTOR_H
#define _ZYSOFT_ZYSOFT_MEMORY_UTIL_ALLOCATOR_SELECTOR_H

#include <memory>

#include <zysoft/zysoft_fwd.h>

namespace zysoft
{

template <typename T>
struct allocator_selector
{
#if defined (ZYSOFT_ALLOCATOR_SELECTOR_USE_ZYSOFT_NEW_ALLOCATOR)
        typedef new_allocator<T>                    allocator_type;
#elif defined(ZYSOFT_ALLOCATOR_SELECTOR_USE_STD_ALLOCATOR)
        typedef std::allocator<T>                   allocator_type;
#else 
# error Error in discrimination. allocator_selector must select either std::allocator, stlsoft::malloc_allocator or stlsoft::new_allocator
#endif 

    typedef std::allocator_traits<T>                allocator_traits_type;
};

} // namespace zysoft

#endif // !_ZYSOFT_ZYSOFT_MEMORY_UTIL_ALLOCATOR_SELECTOR_H

#ifndef _ZYSOFT_ZYSOFT_ZYSOFT_FWD_H
#define _ZYSOFT_ZYSOFT_ZYSOFT_FWD_H

#define ZYSOFT_COMPILER(MAJOR, MINOR, PATCH) ((MAJOR)*10000000 + (MINOR)*100000 + (PATCH))

#if defined(_MSC_VER) && defined(_MSC_FULL_VER)
#   if _MSC_VER == _MSC_FULL_VER / 10000
#       define ZYSOFT_MSVC ZYSOFT_COMPILER(_MSC_VER / 100, _MSC_VER % 100, _MSC_FULL_VER % 10000)
#   else // MSVC
#       define ZYSOFT_MSVC ZYSOFT_COMPILER(_MSC_VER / 100, (_MSC_FULL_VER / 100000) % 100, _MSC_FULL_VER % 100000)
#   endif // MSVC
#endif // MSVC
#if defined(__clang__) && defined(__clang_minor__)
#define ZYSOFT_CLANG ZYSOFT_COMPILER(__clang_major__, __clang_minor__, __clang_patchlevel__)
#elif defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__) && !defined(__INTEL_COMPILER)
#define ZYSOFT_GCC ZYSOFT_COMPILER(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#endif // GCC

#ifndef ZYSOFT_MSVC
#define ZYSOFT_MSVC 0
#endif // ZYSOFT_MSVC
#ifndef ZYSOFT_CLANG
#define ZYSOFT_CLANG 0
#endif // ZYSOFT_CLANG
#ifndef ZYSOFT_GCC
#define ZYSOFT_GCC 0
#endif // ZYSOFT_GCC

#if ZYSOFT_MSVC
#define ZYSOFT_MSVC_SUPPRESS_WARNING_PUSH __pragma(warning(push))
#define ZYSOFT_MSVC_SUPPRESS_WARNING(w) __pragma(warning(disable : w))
#define ZYSOFT_MSVC_SUPPRESS_WARNING_POP __pragma(warning(pop))
#define ZYSOFT_MSVC_SUPPRESS_WARNING_WITH_PUSH(w) ZYSOFT_MSVC_SUPPRESS_WARNING_PUSH ZYSOFT_MSVC_SUPPRESS_WARNING(w)
#else // ZYSOFT_MSVC
#define ZYSOFT_MSVC_SUPPRESS_WARNING_PUSH
#define ZYSOFT_MSVC_SUPPRESS_WARNING(w)
#define ZYSOFT_MSVC_SUPPRESS_WARNING_POP3
#define ZYSOFT_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif // ZYSOFT_MSVC

#if ZYSOFT_MSVC
#   if _MSVC_LANG >= 201703L
#       define ZYSOFT_CXX 2017L
#   elif _MSVC_LANG >= 201402L
#       define ZYSOFT_CXX 2014L
#   endif
#   ifndef ZYSOFT_CXX
#       define ZYSOFT_CXX 2011L
#   endif // !_ZYSOFT_CXX
#endif // ZYSOFT_MSVC

#if __cplusplus >= 201703L
#define ZYSOFT_CXX 2017L
#elif __cplusplus >= 201402L
#define ZYSOFT_CXX 2014L
#elif __cplusplus >= 201103L
#define ZYSOFT_CXX 2011L
#endif // __cplusplus >= 201703L
#ifndef ZYSOFT_CXX
#define ZYSOFT_CXX 1997L
#endif // !ZYSOFT_CXX

#define ZYSOFT_CXX17 2017L
#define ZYSOFT_CXX14 2014L
#define ZYSOFT_CXX11 2011L

#include <cassert>
#include <cstdint>
#include <cstddef>

#define ZYSOFT_ASSERT(expression) assert(expression)
#define ZYSOFT_MESSAGE_ASSERT(expression, m) assert(expression)

#define ZYSOFT_ALLOCATOR_SELECTOR_USE_STD_ALLOCATOR

#define ZYSOFT_NODISCARD            //[[nodiscard]]
#define ZYSOFT_MAYBE_UNUSED         //[[maybe_unused]]
#define ZYSOFT_NOEXCEPT             noexcept

#if ZYSOFT_MSVC
#include <windows.h>
#endif

#if ZYSOFT_GCC
#include <sys/time.h>
#endif

#endif // !_ZYSOFT_ZYSOFT_ZYSOFT_FWD_H


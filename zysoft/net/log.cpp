#include <cstdarg>
#include <iostream>
#include <array>

#include <zysoft/net/log.h>

namespace zysoft 
{

namespace net
{

static log_callback g_log_function = nullptr;
static log_severity g_log_severity = log_severity::trace;
    
void set_log_callback(log_callback cb)
{
    g_log_function = cb;
}

void set_log_severity(log_severity severity)
{
    g_log_severity = severity;
}

static void PrintLog(log_severity severity, const char *msg)
{
    if (g_log_function) {
        g_log_function(severity, msg);
    } else {
        const char *severity_str;
        switch (severity) {
        case log_severity::trace:
            severity_str = "ZYSOFT_NET_TRACE";
            break;
        case log_severity::info:
            severity_str = "ZYSOFT_NET_INFO ";
            break;
        case log_severity::warn:
            severity_str = "ZYSOFT_NET_WARN ";
            break;
        case log_severity::fatal:
            severity_str = "ZYSOFT_NET_FATAL";
            break;
        default:
            severity_str = "???";
            break;
        }
        (void)fprintf(stderr, "[%s] %s\n", severity_str, msg);
    }
}

static int vsnprintf(char* dest, int size, const char* fmt, va_list argptr)
{
    int ret = ::vsnprintf(dest, size - 1, fmt, argptr);
    dest[size - 1] = '\0';
    if (ret < 0 || ret >= size) {
        return -1;
    }
    return ret;
}

void printf(log_severity severity, const char* fmt, ...)
{
    if (severity < g_log_severity)
        return;

    va_list argptr;
    std::array<char, 1024> msg{};

    va_start(argptr, fmt);
    vsnprintf(msg.data(), msg.size(), fmt, argptr);
    va_end(argptr);
    PrintLog(severity, msg.data());
}

} // namespace net

} // namespace zysoft


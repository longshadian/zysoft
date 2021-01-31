#ifndef _ZYSOFT_NET_LOG_H
#define _ZYSOFT_NET_LOG_H

#include <cstdint>
#include <sstream>
#include <string>
#include <iostream>
#include <ctime>


namespace zysoft
{

namespace net
{

enum log_severity
{
    trace   = 0,
    info    = 1,
    warn    = 2,
    fatal   = 3,
    num_severity = 4,
};

using log_callback = void (*)(log_severity severity, const char* msg);
void set_log_callback(log_callback cb);
void set_log_severity(log_severity severity);

void printf(log_severity severity, const char* fmt, ...);

} // namespace net

} // namespace zysoft

#define ZYSOFT_NET_LOG(level, format, ...) zysoft::net::printf(::zysoft::net::log_severity::level, "%d:%s:%s " format, __LINE__, __FILE__, __FUNCTION__, ##__VA_ARGS__)

#define ZYSOFT_NET_LOG_TRACE(format, ...) ZYSOFT_NET_LOG(trace, format, ##__VA_ARGS__);
#define ZYSOFT_NET_LOG_INFO(format, ...) ZYSOFT_NET_LOG(info, format, ##__VA_ARGS__);
#define ZYSOFT_NET_LOG_WARN(format, ...) ZYSOFT_NET_LOG(warn, format, ##__VA_ARGS__);
#define ZYSOFT_NET_LOG_FATAL(format, ...) ZYSOFT_NET_LOG(fatal, format, ##__VA_ARGS__);

#define ZYSOFT_NET_TRACE    ZYSOFT_NET_LOG_TRACE
#define ZYSOFT_NET_INFO     ZYSOFT_NET_LOG_INFO
#define ZYSOFT_NET_WARN     ZYSOFT_NET_LOG_WARN
#define ZYSOFT_NET_FATAL    ZYSOFT_NET_LOG_FATAL

#endif // !_ZYSOFT_NET_LOG_H


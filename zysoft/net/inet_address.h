#ifndef _ZYSOFT_NET_INET_ADDRESS_H
#define _ZYSOFT_NET_INET_ADDRESS_H


#include <cstdint>
#include <string>
#include <sstream>
    
namespace zysoft
{
namespace net
{

class inet_address
{
public:
    inet_address(std::string ip, std::uint16_t port)
        : ip_(std::move(ip))
        , port_(port)
    {
    }

    ~inet_address()
    {
    }

    inet_address(const inet_address&) = default;
    inet_address& operator=(const inet_address&) = default;
    inet_address(inet_address&&) = default;
    inet_address& operator=(inet_address&&) = default;

    std::string ip_port_to_string() const
    {
        std::ostringstream ostm{};
        ostm << ip_ << ":" << port_;
        return ostm.str();
    }

    std::string ip_;
    std::uint16_t port_;
};

inline
std::string create_connection_name(const inet_address& addr, int conn_id)
{
    char buffer[64] = {0};
    snprintf(buffer, sizeof buffer, "%s:%d#%d", addr.ip_.c_str(), static_cast<int>(addr.port_), conn_id);
    return buffer;
}

inline
std::string create_connection_name(const inet_address& addr)
{
    char buffer[64] = {0};
    snprintf(buffer, sizeof buffer, "%s:%d", addr.ip_.c_str(), static_cast<int>(addr.port_));
    return buffer;
}

}  // namespace net
}  // namespace zysoft

#endif // !_ZYSOFT_NET_INET_ADDRESS_H

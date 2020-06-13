#pragma once

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

}  // namespace net
}  // namespace muduo


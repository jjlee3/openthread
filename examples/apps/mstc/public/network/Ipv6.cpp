#include "pch/pch.h"
#include <network/Ipv6.h>

void
mstc::network::Ipv6::socket(
    int addrFamily,
    int type,
    int protocol)
{
    if (addrFamily == AF_INET)
    {
        MSTC_THROW_EXCEPTION(SocketFailure{});
    }
    base_t::socket(addrFamily, type, protocol);
}

void
mstc::network::Ipv6::bind(
    const sockaddr_in6* saddr,
    int                 len) const
{
    base_t::bind(reinterpret_cast<const sockaddr*>(saddr), len);
}

mstc::network::Ipv6
mstc::network::Ipv6::accept(
    sockaddr_in6* saddr,
    int*          len) const
{
    return Ipv6{base_t::accept(reinterpret_cast<sockaddr*>(saddr), len).detach()};
}

void
mstc::network::Ipv6::connect(
    const sockaddr_in6* saddr,
    int                 len) const
{
    base_t::connect(reinterpret_cast<const sockaddr*>(saddr), len);
}

void
mstc::network::Ipv6::getsockname(
    sockaddr_in6* saddr,
    int*          len) const
{
    base_t::getsockname(reinterpret_cast<sockaddr*>(saddr), len);
}
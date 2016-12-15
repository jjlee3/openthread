#pragma once

#include <utility>
#include <network/Socket.h>
#include <ws2ipdef.h>

namespace mstc
{
namespace network
{
class Ipv6 : public Socket
{
    using base_t = Socket;
public:
    EXCEPT_DERIVED(SocketFailure, Failure);

public:
    using base_t::base_t;

    void socket(int addrFamily, int type, int protocol);
    void bind(const sockaddr_in6*, int len) const;
    Ipv6 accept(sockaddr_in6*, int* len) const;
    void connect(const sockaddr_in6*, int len) const;
    void getsockname(sockaddr_in6*, int* len) const;
}; // Ipv6
} // network
} // mstc
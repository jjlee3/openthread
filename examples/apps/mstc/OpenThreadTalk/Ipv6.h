#pragma once

#include "Types.h"

struct in6_addr;

namespace OpenThreadTalk
{
    class Ipv6
    {
    public:
        // according to ::RtlIpv6AddressToString spec
        static constexpr size_t IPV6_ADDR_BUF_SIZE = 46;

        // not a complete check but it is OK
        static bool IsValidAddress(String^ ipv6Addr);

        // format Ipv6 address to string
        static int ToStringA(char* buffer, size_t sizeBuffer, const in6_addr& in6addr);

        // format Ipv6 address to string
        static int ToStringW(wchar_t* buffer, size_t sizeBuffer, const in6_addr& in6addr);
    };
}

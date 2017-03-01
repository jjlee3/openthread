#pragma once

#include "Types.h"

struct in6_addr;

namespace OpenThreadTalk
{
    class Ipv6
    {
    public:
        // not a complete check but it is OK
        static bool IsValidAddress(String^ ipv6Addr);

        // format Ipv6 address to string
        static int ToStringA(char* buffer, size_t sizeBuffer, const in6_addr& in6addr);

        // format Ipv6 address to string
        static int ToStringW(wchar_t* buffer, size_t sizeBuffer, const in6_addr& in6addr);
    };
}

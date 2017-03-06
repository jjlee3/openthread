#include "pch.h"
#include <algorithm>
#include "Ipv6.h"

using namespace OpenThreadTalk;

bool
Ipv6::IsValidAddress(
    String^ ipv6Addr)
{
    // 0 ~ 9 : a ~ f
    return std::all_of(ipv6Addr->Data(), ipv6Addr->Data() + ipv6Addr->Length(),
        [](wchar_t c)
    {
        if (c < '0')
        {
        }
        else if (c <= '9')
        {
            return true; // 0 ~ 9
        }
        else if (c <= ':')
        {
            return true; // :
        }
        else if (c < 'a')
        {
        }
        else if (c <= 'f')
        {
            return true; // a ~ f
        }
        else
        {
        }

        return false;
    });
}

int
Ipv6::ToStringA(
    char*           buffer,
    size_t          sizeBuffer,
    const in6_addr& in6addr)
{
    ::RtlIpv6AddressToStringA(&in6addr, buffer);
    return static_cast<int>(strlen(buffer));
}

int
Ipv6::ToStringW(
    wchar_t*        buffer,
    size_t          sizeBuffer,
    const in6_addr& in6addr)
{
    ::RtlIpv6AddressToStringW(&in6addr, buffer);
    return static_cast<int>(wcslen(buffer));
}

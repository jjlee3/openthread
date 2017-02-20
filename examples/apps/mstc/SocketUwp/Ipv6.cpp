#include "pch.h"
#include <string>
#include "Ipv6.h"

bool
SocketUwp::Ipv6::IsValidAddress(
    String^ ipv6Addr)
{
    // 0 ~ 9 : a ~ f A ~ F
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
        else if (c < 'A')
        {
        }
        else if (c <= 'F')
        {
            return true; // A ~ F
        }
        else
        {
        }

        return false;
    });
}

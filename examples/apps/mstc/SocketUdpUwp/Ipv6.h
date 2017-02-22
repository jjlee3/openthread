#pragma once

#include "Types.h"

namespace SocketUdpUwp
{
    class Ipv6
    {
    public:
        // not a complete check but it is OK
        static bool IsValidAddress(String^ ipv6Addr);
    };
}
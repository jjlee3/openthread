#pragma once

#include "Types.h"

namespace SocketUwp
{
    public ref class ClientContextArgs sealed
    {
    public:
        property HostName^ ServerHostName;
        property Service^  ServerPort;
        property HostName^ ClientHostName;
        property Service^  ClientPort;
    };
}

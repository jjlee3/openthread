#pragma once

#include "Types.h"

namespace SocketUwp
{
    public ref class ClientArgs sealed
    {
    public:
        property HostName^ ServerHostName;
        property Service^  ServerPort;
        property HostName^ ClientHostName;
        property Service^  ClientPort;
    };
}

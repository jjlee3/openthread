#pragma once

#include "Types.h"

namespace SocketUwp
{
    public ref class ListenerArgs sealed
    {
    public:
        property String^   ServerName;
        property HostName^ ServerHostName;
        property Service^  ServerPort;
    };
}

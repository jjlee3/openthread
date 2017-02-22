#pragma once

#include "Types.h"

namespace OpenThreadTalk
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

#pragma once

#include "Types.h"

namespace OpenThreadTalk
{
    public ref class ListenerArgs sealed
    {
    public:
        property String^   ServerName;
        property HostName^ ServerHostName;
        property Service^  ServerPort;
    };
}

#pragma once

#include "IAsyncThreadPage.h"
#include "ListenerArgs.h"
#include "IListenerContext.h"
#include "ClientArgs.h"
#include "IClientContext.h"

namespace OpenThreadTalk
{
    class Factory
    {
    public:
        static IListenerContext^ CreateListenerContext(IAsyncThreadPage^, ListenerArgs^);

        static IClientContext^ CreateClientContext(IAsyncThreadPage^, ClientArgs^);
    };
}

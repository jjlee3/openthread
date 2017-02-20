#pragma once

#include "IAsyncThreadPage.h"
#include "ListenerContextArgs.h"
#include "IListenerContext.h"
#include "ClientContextArgs.h"
#include "IClientContext.h"

namespace SocketUwp
{
    class Factory
    {
    public:
        static IListenerContext^ CreateListenerContext(IAsyncThreadPage^, ListenerContextArgs^);

        static IClientContext^ CreateClientContext(IAsyncThreadPage^, ClientContextArgs^);
    };
}

#include "pch.h"
#include "Factory.h"
#include "StreamListenerContext.h"
#include "DatagramListenerContext.h"
#include "StreamClientContext.h"
#include "DatagramClientContext.h"
#include "Configurations.h"

using namespace SocketUwp;

IListenerContext^
Factory::CreateListenerContext(
    IAsyncThreadPage^    page,
    ListenerContextArgs^ listenerContextArgs)
{
    if (g_configurations.protocol == Protocol::TCP)
    {
        auto listener = ref new StreamSocketListener();
        return ref new StreamListenerContext(page, listener, listenerContextArgs);
    }
    else
    {
        auto listener = ref new DatagramSocket();
        return ref new DatagramListenerContext(page, listener, listenerContextArgs);
    }
}

IClientContext^
Factory::CreateClientContext(
    IAsyncThreadPage^  page,
    ClientContextArgs^ clientContextArgs)
{
    if (g_configurations.protocol == Protocol::TCP)
    {
        auto client = ref new StreamSocket();
        return ref new StreamClientContext(page, client, clientContextArgs);
    }
    else
    {
        auto client = ref new DatagramSocket();
        return ref new DatagramClientContext(page, client, clientContextArgs);
    }
}

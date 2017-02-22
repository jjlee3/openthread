#include "pch.h"
#include "Factory.h"
#include "StreamListenerContext.h"
#include "DatagramListenerContext.h"
#include "StreamClientContext.h"
#include "DatagramClientContext.h"
#include "Configurations.h"

using namespace OpenThreadTalk;

IListenerContext^
Factory::CreateListenerContext(
    IAsyncThreadPage^ page,
    ListenerArgs^     listenerArgs)
{
    if (g_configurations.protocol == Protocol::TCP)
    {
        auto listener = ref new StreamSocketListener();
        return ref new StreamListenerContext(page, listener, listenerArgs);
    }
    else
    {
        auto listener = ref new DatagramSocket();
        return ref new DatagramListenerContext(page, listener, listenerArgs);
    }
}

IClientContext^
Factory::CreateClientContext(
    IAsyncThreadPage^ page,
    ClientArgs^       clientArgs)
{
    if (g_configurations.protocol == Protocol::TCP)
    {
        auto client = ref new StreamSocket();
        return ref new StreamClientContext(page, client, clientArgs);
    }
    else
    {
        auto client = ref new DatagramSocket();
        return ref new DatagramClientContext(page, client, clientArgs);
    }
}

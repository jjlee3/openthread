#pragma once

#include "Types.h"
#include "IAsyncThreadPage.h"

namespace SocketUdpUwp
{
    namespace WFM = Windows::Foundation::Metadata;

    [WFM::WebHostHidden]
    public ref class ListenerContext sealed
    {
    public:
        ListenerContext(IAsyncThreadPage^ page, DatagramSocket^ listener, String^ serverName);

        void OnMessage(DatagramSocket^ socket, MessageReceivedEventArgs^ eventArgs);

    private:
        ~ListenerContext();

        void EchoMessage(MessageReceivedEventArgs^ eventArgs);

        DataWriter^ GetDataWriter();

        IAsyncThreadPage^ page_;
        DatagramSocket^   listener_;
        String^           serverName_;

        CRITICAL_SECTION lock_;
        IOutputStream^   outputStream_;
        DataWriter^      dataWriter_;
    };
}

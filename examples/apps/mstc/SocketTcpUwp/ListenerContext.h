#pragma once

#include "Types.h"
#include "IAsyncThreadPage.h"

namespace SocketTcpUwp
{
    namespace WFM = Windows::Foundation::Metadata;

    [WFM::WebHostHidden]
    public ref class ListenerContext sealed
    {
    public:
        ListenerContext(IAsyncThreadPage^ page, StreamSocketListener^ listener, String^ serverName);

        void OnConnection(StreamSocketListener^ listener, ConnectionReceivedEventArgs^ args);

    private:
        ~ListenerContext();

        void ReceiveLoop(StreamSocket^, DataReader^, DataWriter^);

        void EchoMessage(DataReader^, DataWriter^);

        IAsyncThreadPage^     page_;
        StreamSocketListener^ listener_;
        String^               serverName_;
    };
}
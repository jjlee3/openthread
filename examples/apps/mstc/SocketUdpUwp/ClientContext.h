#pragma once

#pragma once

#include "Types.h"
#include "IAsyncThreadPage.h"

namespace SocketUdpUwp
{
    namespace WFM = Windows::Foundation::Metadata;

    [WFM::WebHostHidden]
    public ref class ClientContext sealed
    {
    public:
        ClientContext(IAsyncThreadPage^ page, DatagramSocket^ client);

        void SendMessage(String^ msg);

        void OnMessage(DatagramSocket^ socket, MessageReceivedEventArgs^ eventArgs);

        void SetConnected();

        bool IsConnected();

    private:
        ~ClientContext();

        DataWriter^ GetDataWriter();

        IAsyncThreadPage^ page_;
        DatagramSocket^   client_;
        bool              connected_ = false;
        DataWriter^       dataWriter_;
    };
}

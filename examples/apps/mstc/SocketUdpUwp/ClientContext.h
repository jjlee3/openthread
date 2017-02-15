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

        void OnMessage(DatagramSocket^ socket, DsMessageReceivedEventArgs^ eventArgs);

        void SetConnected();

        bool IsConnected();

    private:
        ~ClientContext();

        IAsyncThreadPage^ page_;
        DatagramSocket^   client_;
        bool              connected_ = false;

    };
}

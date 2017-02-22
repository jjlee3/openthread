#pragma once

#include "Types.h"
#include "IAsyncThreadPage.h"

namespace SocketTcpUwp
{
    [WFM::WebHostHidden]
    public ref class ClientContext sealed
    {
    public:
        ClientContext(IAsyncThreadPage^ page, StreamSocket^ client);

        void OnConnection(StreamSocket^ client);

        void SendMessage(String^ msg);

    private:
        ~ClientContext();

        bool IsConnected() { return connected_; }

        DataReader^ GetDataReader();
        DataWriter^ GetDataWriter();

        void ReceiveLoop(StreamSocket^, DataReader^);

        IAsyncThreadPage^ page_;
        StreamSocket^     client_;
        bool              connected_ = false;
        DataReader^       dataReader_;
        DataWriter^       dataWriter_;
    };
}
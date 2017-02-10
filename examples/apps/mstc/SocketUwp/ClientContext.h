#pragma once

#include "IAppSocket.h"
#include "IAsyncThreadPage.h"

namespace SocketUwp
{
    namespace WFM = Windows::Foundation::Metadata;

    [WFM::WebHostHidden]
    public ref class ClientContext sealed
    {
    public:
        using DataWriter = Windows::Storage::Streams::DataWriter;
        using String = Platform::String;

        ClientContext(IAsyncThreadPage^ page, IAppSocket^ client);
        virtual ~ClientContext();

        DataWriter^ GetDataWriter();

        void SendMessage(String^ msg);

        void OnMessage(IAppSocket^ socket, MessageReceivedEventArgs^ eventArgs);

    private:
        IAsyncThreadPage^ page_;
        IAppSocket^       client_;
        DataWriter^       dataWriter_;
    };
}
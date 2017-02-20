#pragma once

#include "Types.h"
#include "IClientContext.h"
#include "IAsyncThreadPage.h"
#include "ClientContextArgs.h"
#include "StreamClientContextHelper.h"

namespace SocketUwp
{
    namespace WFM = Windows::Foundation::Metadata;

    [WFM::WebHostHidden]
    public ref class StreamClientContext sealed : public IClientContext
    {
    public:
        StreamClientContext(IAsyncThreadPage^ page, StreamSocket^ client, ClientContextArgs^ args);
        virtual ~StreamClientContext();

        virtual void Connect_Click(Object^ sender, RoutedEventArgs^ e);

        virtual void Send_Click(Object^ sender, RoutedEventArgs^ e, String^ input);

    private:
        void OnConnection(StreamSocket^ streamSocket);

        DataReader^ GetDataReader();
        DataWriter^ GetDataWriter();

        ClientContextArgs^        args_;
        StreamClientContextHelper streamClientContextHelper_;
        StreamSocket^             client_;
        DataReader^               dataReader_;
        DataWriter^               dataWriter_;
    };
}
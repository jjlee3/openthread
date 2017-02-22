#pragma once

#include "Types.h"
#include "IClientContext.h"
#include "IAsyncThreadPage.h"
#include "ClientArgs.h"
#include "StreamClientContextHelper.h"

namespace SocketUwp
{
    [WFM::WebHostHidden]
    public ref class StreamClientContext sealed : public IClientContext
    {
    public:
        StreamClientContext(IAsyncThreadPage^ page, StreamSocket^ client, ClientArgs^ args);
        virtual ~StreamClientContext();

        virtual void Connect_Click(Object^ sender, RoutedEventArgs^ e);

        virtual void Send_Click(Object^ sender, RoutedEventArgs^ e, String^ input);

    private:
        using Args = ClientArgs;
        using Helper = StreamClientContextHelper;

        void OnConnection(StreamSocket^ streamSocket);

        DataReader^ GetDataReader();
        DataWriter^ GetDataWriter();

        StreamSocket^ client_;
        ClientArgs^   args_;
        Helper        helper_;
        DataReader^   dataReader_;
        DataWriter^   dataWriter_;
    };
}
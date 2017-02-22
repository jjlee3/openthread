#pragma once

#include "Types.h"
#include "IClientContext.h"
#include "IAsyncThreadPage.h"
#include "ClientArgs.h"
#include "ClientContextHelper.h"

namespace OpenThreadTalk
{
    [WFM::WebHostHidden]
    public ref class DatagramClientContext sealed : public IClientContext
    {
    public:
        DatagramClientContext(IAsyncThreadPage^ page, DatagramSocket^ client, ClientArgs^ args);
        virtual ~DatagramClientContext();

        virtual void Connect_Click(Object^ sender, RoutedEventArgs^ e);

        virtual void Send_Click(Object^ sender, RoutedEventArgs^ e, String^ input);

    private:
        using Args = ClientArgs;
        using Helper = ClientContextHelper;

        void OnMessage(DatagramSocket^ socket, MessageReceivedEventArgs^ eventArgs);

        DataWriter^ GetDataWriter();

        DatagramSocket^ client_;
        ClientArgs^     args_;
        Helper          helper_;
        DataReader^     dataReader_;
        DataWriter^     dataWriter_;
    };
}
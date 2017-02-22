#pragma once

#include "Types.h"
#include "IClientContext.h"
#include "IAsyncThreadPage.h"
#include "ClientContextArgs.h"
#include "ClientContextHelper.h"

namespace SocketUwp
{
    [WFM::WebHostHidden]
    public ref class DatagramClientContext sealed : public IClientContext
    {
    public:
        DatagramClientContext(IAsyncThreadPage^ page, DatagramSocket^ client, ClientContextArgs^ args);
        virtual ~DatagramClientContext();

        virtual void Connect_Click(Object^ sender, RoutedEventArgs^ e);

        virtual void Send_Click(Object^ sender, RoutedEventArgs^ e, String^ input);

    private:
        void OnMessage(DatagramSocket^ socket, MessageReceivedEventArgs^ eventArgs);

        DataWriter^ GetDataWriter();

        ClientContextArgs^  args_;
        ClientContextHelper clientContextHelper_;
        DatagramSocket^     client_;
        DataReader^         dataReader_;
        DataWriter^         dataWriter_;
    };
}
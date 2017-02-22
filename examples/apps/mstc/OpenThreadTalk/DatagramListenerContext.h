#pragma once

#include "Types.h"
#include "IListenerContext.h"
#include "IAsyncThreadPage.h"
#include "ListenerArgs.h"
#include "ListenerContextHelper.h"

namespace OpenThreadTalk
{
    [WFM::WebHostHidden]
    public ref class DatagramListenerContext sealed : public IListenerContext
    {
    public:
        DatagramListenerContext(IAsyncThreadPage^ page, DatagramSocket^ listener, ListenerArgs^ args);
        virtual ~DatagramListenerContext();

        virtual void Listen_Click(Object^ sender, RoutedEventArgs^ e);

    private:
        using Listener = DatagramSocket;
        using Args = ListenerArgs;
        using Helper = ListenerContextHelper;

        void OnMessage(DatagramSocket^ socket, MessageReceivedEventArgs^ eventArgs);

        DataWriter^ GetDataWriter();

        Listener^ listener_;
        Args^     args_;
        Helper    helper_;

        CRITICAL_SECTION lock_;
        IOutputStream^   outputStream_;
        DataWriter^      dataWriter_;
    };
}
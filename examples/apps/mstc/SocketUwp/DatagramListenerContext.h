#pragma once

#include "Types.h"
#include "IListenerContext.h"
#include "IAsyncThreadPage.h"
#include "ListenerContextArgs.h"
#include "ListenerContextHelper.h"

namespace SocketUwp
{
    namespace WFM = Windows::Foundation::Metadata;

    [WFM::WebHostHidden]
    public ref class DatagramListenerContext sealed : public IListenerContext
    {
    public:
        DatagramListenerContext(IAsyncThreadPage^ page, DatagramSocket^ listener, ListenerContextArgs^ args);
        virtual ~DatagramListenerContext();

        virtual void Listen_Click(Object^ sender, RoutedEventArgs^ e);

    private:
        void OnMessage(DatagramSocket^ socket, MessageReceivedEventArgs^ eventArgs);

        DataWriter^ GetDataWriter();

        ListenerContextArgs^  args_;
        ListenerContextHelper listenerContextHelper_;
        DatagramSocket^       listener_;

        CRITICAL_SECTION lock_;
        IOutputStream^   outputStream_;
        DataWriter^      dataWriter_;
    };
}
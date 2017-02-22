#pragma once

#include "Types.h"
#include "IListenerContext.h"
#include "IAsyncThreadPage.h"
#include "ListenerContextArgs.h"
#include "StreamListenerContextHelper.h"

namespace SocketUwp
{
    [WFM::WebHostHidden]
    public ref class StreamListenerContext sealed : public IListenerContext
    {
    public:
        StreamListenerContext(IAsyncThreadPage^ page, StreamSocketListener^ listener, ListenerContextArgs^ args);
        virtual ~StreamListenerContext();

        virtual void Listen_Click(Object^ sender, RoutedEventArgs^ e);

    private:
        void OnConnection(StreamSocketListener^ listener, ConnectionReceivedEventArgs^ args);

        ListenerContextArgs^        args_;
        StreamListenerContextHelper streamListenerContextHelper_;
        StreamSocketListener^       listener_;
    };
}
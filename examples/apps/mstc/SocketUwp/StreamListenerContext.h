#pragma once

#include "Types.h"
#include "IListenerContext.h"
#include "IAsyncThreadPage.h"
#include "ListenerArgs.h"
#include "StreamListenerContextHelper.h"

namespace SocketUwp
{
    [WFM::WebHostHidden]
    public ref class StreamListenerContext sealed : public IListenerContext
    {
    public:
        StreamListenerContext(IAsyncThreadPage^ page, StreamSocketListener^ listener, ListenerArgs^ args);
        virtual ~StreamListenerContext();

        virtual void Listen_Click(Object^ sender, RoutedEventArgs^ e);

    private:
        using Listener = StreamSocketListener;
        using Args = ListenerArgs;
        using Helper = StreamListenerContextHelper;

        void OnConnection(StreamSocketListener^ listener, ConnectionReceivedEventArgs^ args);

        Listener^ listener_;
        Args^     args_;
        Helper    helper_;
    };
}
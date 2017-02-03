#pragma once

#include "IAppSocket.h"
#include "IAsyncThreadPage.h"
#include "MessageReceivedEventArgs.h"

namespace SocketUwp
{
    namespace WFM = Windows::Foundation::Metadata;

    [WFM::WebHostHidden]
    public ref class ListenerContext sealed
    {
    public:
        using IOutputStream = Windows::Storage::Streams::IOutputStream;
        using String = Platform::String;

        ListenerContext(IAsyncThreadPage^ page, IAppSocket^ listener, String^ serverName);
        virtual ~ListenerContext();

        void OnMessage(IAppSocket^ socket, MessageReceivedEventArgs^ eventArgs);

    private:
        void EchoMessage(MessageReceivedEventArgs^ eventArgs);

        IAsyncThreadPage^ page_;
        IAppSocket^       listener_;
        String^           serverName_;

        CRITICAL_SECTION lock_;
        IOutputStream^   outputStream_;
    };
}
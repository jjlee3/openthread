
#pragma once

#include "MessageReceivedEventArgs.h"

namespace SocketUwp
{
    public interface struct IAppSocket
    {
        using HostName = Windows::Networking::HostName;
        using String = Platform::String;
        using Service = String;
        using IAsyncAction = Windows::Foundation::IAsyncAction;
        using EndpointPair = Windows::Networking::EndpointPair;
        using IOutputStream = Windows::Storage::Streams::IOutputStream;
        using IAsyncOutputStream = Windows::Foundation::IAsyncOperation<IOutputStream^>;
        using DatagramSocket = Windows::Networking::Sockets::DatagramSocket;
        using MessageHandler = Windows::Foundation::TypedEventHandler<IAppSocket^, MessageReceivedEventArgs^>;
        
        IAsyncAction^ BindEndpointAsync(HostName^ localHostName, Service^ localService);

        IAsyncAction^ ConnectAsync(EndpointPair^ endpointPair);
        IAsyncAction^ ConnectAsync(HostName^ remoteHostName, Service^ remoteService);

        void SetConnected();

        bool IsConnected();

        IAsyncOutputStream^ GetOutputStreamAsync(HostName^ remoteHostName, Service^ remoteService);

        event MessageHandler^ MessageReceived;

        property IOutputStream^ OutputStream
        {
            IOutputStream^ get();
        }
    };
}
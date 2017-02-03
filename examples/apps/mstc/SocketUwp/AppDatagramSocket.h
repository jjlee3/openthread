#pragma once

#include "IAppSocket.h"

namespace SocketUwp
{
    namespace WFM = Windows::Foundation::Metadata;

    [WFM::WebHostHidden]
    public ref class AppDatagramSocket sealed : public IAppSocket
    {
    public:
        using HostName = IAppSocket::HostName;
        using Service = IAppSocket::Service;
        using IAsyncAction = IAppSocket::IAsyncAction;
        using EndpointPair = IAppSocket::EndpointPair;
        using IOutputStream = IAppSocket::IOutputStream;
        using IAsyncOutputStream = IAppSocket::IAsyncOutputStream;
        using MessageHandler = IAppSocket::MessageHandler;

        AppDatagramSocket();
        virtual ~AppDatagramSocket();

        virtual IAsyncAction^ BindEndpointAsync(HostName^ localHostName, Service^ localService);

        virtual IAsyncAction^ ConnectAsync(EndpointPair^ endpointPair);
        virtual IAsyncAction^ ConnectAsync(HostName^ remoteHostName, Service^ remoteService);

        virtual void SetConnected();

        virtual bool IsConnected();

        virtual IAsyncOutputStream^ GetOutputStreamAsync(HostName^ remoteHostName, Service^ remoteService);

        virtual event MessageHandler^ MessageReceived;

        virtual property IOutputStream^ OutputStream
        {
            IOutputStream^ get();
        }

    private:
        using DatagramSocket = Windows::Networking::Sockets::DatagramSocket;
        using DatagramSocketMessageReceivedEventArgs = Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs;
        using DatagramSocketMessageHandler = Windows::Foundation::TypedEventHandler<DatagramSocket^, DatagramSocketMessageReceivedEventArgs^>;

        void OnMessage(DatagramSocket^ datagramSocket, DatagramSocketMessageReceivedEventArgs^ args);

        DatagramSocket^ sock_;
        Service^        localPort_;
        bool            connected_ = false;
    };
}
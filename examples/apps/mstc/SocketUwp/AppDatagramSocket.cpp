#include "pch.h"
#include "AppDatagramSocket.h"

SocketUwp::AppDatagramSocket::AppDatagramSocket() :
    sock_{ ref new DatagramSocket() }
{
    sock_->MessageReceived += ref new DatagramSocketMessageHandler(
        this, &AppDatagramSocket::OnMessage);
}

SocketUwp::AppDatagramSocket::~AppDatagramSocket()
{
    if (sock_ != nullptr)
    {
        delete sock_;
        sock_ = nullptr;
    }
}

Windows::Foundation::IAsyncAction^
SocketUwp::AppDatagramSocket::BindEndpointAsync(
    HostName^ localHostName, Service^ localService)
{
    localPort_ = localService;
    return sock_->BindEndpointAsync(localHostName, localService);
}

Windows::Foundation::IAsyncAction^
SocketUwp::AppDatagramSocket::ConnectAsync(
    EndpointPair^ endpointPair)
{
    return sock_->ConnectAsync(endpointPair);
}

Windows::Foundation::IAsyncAction^
SocketUwp::AppDatagramSocket::ConnectAsync(
    HostName^ remoteHostName, Service^ remoteService)
{
    return sock_->ConnectAsync(remoteHostName, remoteService);
}

void
SocketUwp::AppDatagramSocket::SetConnected()
{
    connected_ = true;
}

bool
SocketUwp::AppDatagramSocket::IsConnected()
{
    return connected_;
}

Windows::Foundation::IAsyncOperation<Windows::Storage::Streams::IOutputStream^>^
SocketUwp::AppDatagramSocket::GetOutputStreamAsync(
    HostName^ remoteHostName, Service^ remoteService)
{
    return sock_->GetOutputStreamAsync(remoteHostName, remoteService);
}

void
SocketUwp::AppDatagramSocket::OnMessage(
    DatagramSocket^ datagramSocket, DatagramSocketMessageReceivedEventArgs^ args)
{
    MessageReceived(this, ref new MessageReceivedEventArgs(args, localPort_));
}

SocketUwp::AppDatagramSocket::IOutputStream^
SocketUwp::AppDatagramSocket::OutputStream::get()
{
    return sock_->OutputStream;
}

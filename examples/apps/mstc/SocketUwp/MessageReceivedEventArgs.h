#pragma once

namespace SocketUwp
{
    namespace WFM = Windows::Foundation::Metadata;

    [WFM::WebHostHidden]
    public ref class MessageReceivedEventArgs sealed
    {
    public:
        using DatagramSocketMessageReceivedEventArgs = Windows::Networking::Sockets::DatagramSocketMessageReceivedEventArgs;
        using HostName = Windows::Networking::HostName;
        using String = Platform::String;
        using Service = String;
        using DataReader = Windows::Storage::Streams::DataReader;

        MessageReceivedEventArgs(DatagramSocketMessageReceivedEventArgs^ args, Service^ localPort);

        property HostName^ LocalHost;
        property Service^  LocalPort;
        property HostName^ RemoteHost;
        property Service^  RemotePort;

        DataReader^ GetDataReader();

    private:
        DataReader^ dataReader_;
    };
}
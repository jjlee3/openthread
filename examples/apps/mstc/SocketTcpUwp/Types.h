#pragma once

namespace SocketTcpUwp
{
    namespace WN = Windows::Networking;
    namespace WNS = Windows::Networking::Sockets;
    namespace WF = Windows::Foundation;
    namespace WFM = Windows::Foundation::Metadata;
    namespace WSS = Windows::Storage::Streams;

    using HostName = WN::HostName;
    using String = Platform::String;
    using Service = String;
    using StreamSocketListener = WNS::StreamSocketListener;
    using ConnectionReceivedEventArgs = WNS::StreamSocketListenerConnectionReceivedEventArgs;
    using ConnectionHandler = WF::TypedEventHandler<StreamSocketListener^, ConnectionReceivedEventArgs^>;
    using StreamSocket = WNS::StreamSocket;
    using DataReader = WSS::DataReader;
    using DataWriter = WSS::DataWriter;
    using EndpointPair = WN::EndpointPair;
}
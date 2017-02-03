#include "pch.h"
#include "MessageReceivedEventArgs.h"

SocketUwp::MessageReceivedEventArgs::MessageReceivedEventArgs(
    DatagramSocketMessageReceivedEventArgs^ args,
    Service^                                localPort)
{
    LocalHost = args->LocalAddress;
    LocalPort = localPort;
    RemoteHost = args->RemoteAddress;
    RemotePort = args->RemotePort;
    dataReader_ = args->GetDataReader();
}

Windows::Storage::Streams::DataReader^
SocketUwp::MessageReceivedEventArgs::GetDataReader()
{
    return dataReader_;
}

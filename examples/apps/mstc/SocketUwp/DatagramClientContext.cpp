#include "pch.h"
#include "DatagramClientContext.h"

using namespace Concurrency;

SocketUwp::DatagramClientContext::DatagramClientContext(
    IAsyncThreadPage^  page,
    DatagramSocket^    client,
    ClientContextArgs^ args) :
    args_{ std::move(args) },
    clientContextHelper_{ std::move(page) },
    client_{ std::move(client) }
{
}

SocketUwp::DatagramClientContext::~DatagramClientContext()
{
    // A Client can be closed in two ways:
    //  - explicitly: using the 'delete' keyword (client is closed even if there are outstanding references to it).
    //  - implicitly: removing the last reference to it (i.e., falling out-of-scope).
    //
    // When a Socket is closed implicitly, it can take several seconds for the local port being used
    // by it to be freed/reclaimed by the lower networking layers. During that time, other sockets on the machine
    // will not be able to use the port. Thus, it is strongly recommended that Socket instances be explicitly
    // closed before they go out of scope(e.g., before application exit). The call below explicitly closes the socket.
    if (client_ != nullptr)
    {
        delete client_;
        client_ = nullptr;
    }
}

void
SocketUwp::DatagramClientContext::Connect_Click(
    Object^          sender,
    RoutedEventArgs^ e)
{
    if (CoreApplication::Properties->HasKey("clientContext"))
    {
        CoreApplication::Properties->Remove("clientContext");
    }

    client_->MessageReceived += ref new MessageHandler(
        this, &DatagramClientContext::OnMessage);

    // Events cannot be hooked up directly to the ScenarioInput2 object, as the object can fall out-of-scope and be
    // deleted. This would render any event hooked up to the object ineffective. The ClientContext guarantees that
    // both the socket and object that serves its events have the same lifetime.
    CoreApplication::Properties->Insert("clientContext", this);

    auto endpointPair = ref new EndpointPair(args_->ClientHostName, args_->ClientPort,
        args_->ServerHostName, args_->ServerPort);

    create_task(client_->ConnectAsync(endpointPair)).then(
        [this, endpointPair](task<void> prevTask)
    {
        try
        {
            // Try getting an exception.
            prevTask.get();
            clientContextHelper_.NotifyFromAsyncThread(
                "Connect from " + endpointPair->LocalHostName->CanonicalName +
                " to " + endpointPair->RemoteHostName->CanonicalName,
                NotifyType::Status);
            clientContextHelper_.SetConnected(true);
        }
        catch (Exception^ ex)
        {
            CoreApplication::Properties->Remove("clientContext");
            clientContextHelper_.NotifyFromAsyncThread(
                "Start binding failed with error: " + ex->Message,
                NotifyType::Error);
        }
        catch (task_canceled&)
        {
            CoreApplication::Properties->Remove("clientContext");
        }
    });
}

void
SocketUwp::DatagramClientContext::Send_Click(
    Object^          sender,
    RoutedEventArgs^ e,
    String^          input)
{
    clientContextHelper_.SendMessage(GetDataWriter(), false, input);
}

void
SocketUwp::DatagramClientContext::OnMessage(
    DatagramSocket^           socket,
    MessageReceivedEventArgs^ eventArgs)
{
    try
    {
        auto dataReader = eventArgs->GetDataReader();
        clientContextHelper_.Receive(dataReader, dataReader->UnconsumedBufferLength);
    }
    catch (Exception^ ex)
    {
        auto socketError = SocketError::GetStatus(ex->HResult);
        if (socketError == SocketErrorStatus::ConnectionResetByPeer)
        {
            // This error would indicate that a previous send operation resulted in an ICMP "Port Unreachable" message.
            clientContextHelper_.NotifyFromAsyncThread(
                "Peer does not listen on the specific port. Please make sure that you run step 1 first " +
                "or you have a server properly working on a remote server.",
                NotifyType::Error);
        }
        else if (socketError != SocketErrorStatus::Unknown)
        {
            clientContextHelper_.NotifyFromAsyncThread(
                "Error happened when receiving a datagram: " + socketError.ToString(),
                NotifyType::Error);
        }
        else
        {
            throw;
        }
    }
}

SocketUwp::DataWriter^
SocketUwp::DatagramClientContext::GetDataWriter()
{
    if (dataWriter_ == nullptr)
    {
        dataWriter_ = ref new DataWriter(client_->OutputStream);
    }

    return dataWriter_;
}

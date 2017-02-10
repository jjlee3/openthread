#include "pch.h"
#include "ClientContext.h"

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Networking::Sockets;

SocketUwp::ClientContext::ClientContext(
    IAsyncThreadPage^ page,
    IAppSocket^       client)
    : page_{std::move(page)}, client_{std::move(client)}
{
}

SocketUwp::ClientContext::~ClientContext()
{
    // A DatagramSocket can be closed in two ways:
    //  - explicitly: using the 'delete' keyword (listener is closed even if there are outstanding references to it).
    //  - implicitly: removing the last reference to it (i.e., falling out-of-scope).
    //
    // When a DatagramSocket is closed implicitly, it can take several seconds for the local UDP port being used
    // by it to be freed/reclaimed by the lower networking layers. During that time, other UDP sockets on the machine
    // will not be able to use the port. Thus, it is strongly recommended that DatagramSocket instances be explicitly
    // closed before they go out of scope(e.g., before application exit). The call below explicitly closes the socket.

    if (client_ != nullptr)
    {
        delete client_;
        client_ = nullptr;
    }
}

Windows::Storage::Streams::DataWriter^
SocketUwp::ClientContext::GetDataWriter()
{
    if (dataWriter_ == nullptr)
    {
        dataWriter_ = ref new DataWriter(client_->OutputStream);
    }

    return dataWriter_;
}

void
SocketUwp::ClientContext::SendMessage(
    String^ msg)
{
    if (!client_->IsConnected())
    {
        page_->NotifyFromAsyncThread("This socket is not yet connected.", NotifyType::Error);
        return;
    }

    auto writer = GetDataWriter();

    try
    {
        writer->WriteString(msg);
        page_->NotifyFromAsyncThread("Sending - " + msg, NotifyType::Status);
    }
    catch (Exception^ ex)
    {
        page_->NotifyFromAsyncThread("Send failed with error: " + ex->Message, NotifyType::Error);
    }

    // Write the locally buffered data to the network. Please note that write operation will succeed
    // even if the server is not listening.
    create_task(writer->StoreAsync()).then(
        [this](task<unsigned int> writeTask)
    {
        try
        {
            // Try getting an exception.
            writeTask.get();
        }
        catch (Exception^ ex)
        {
            page_->NotifyFromAsyncThread("Send failed with error: " + ex->Message, NotifyType::Error);
        }
    });
}

void
SocketUwp::ClientContext::OnMessage(
    IAppSocket^               socket,
    MessageReceivedEventArgs^ eventArgs)
{
    try
    {
        auto reader = eventArgs->GetDataReader();
        auto strLen = static_cast<unsigned int>(reader->UnconsumedBufferLength);
        if (!strLen) { return; }

        auto msg = reader->ReadString(strLen);

        page_->NotifyFromAsyncThread(
            "Received data from server peer: \"" + msg + "\"",
            NotifyType::Status);
    }
    catch (Exception^ ex)
    {
        auto socketError = SocketError::GetStatus(ex->HResult);
        if (socketError == SocketErrorStatus::ConnectionResetByPeer)
        {
            // This error would indicate that a previous send operation resulted in an ICMP "Port Unreachable" message.
            page_->NotifyFromAsyncThread(
                "Peer does not listen on the specific port. Please make sure that you run step 1 first " +
                "or you have a server properly working on a remote server.",
                NotifyType::Error);
        }
        else if (socketError != SocketErrorStatus::Unknown)
        {
            page_->NotifyFromAsyncThread(
                "Error happened when receiving a datagram: " + socketError.ToString(),
                NotifyType::Error);
        }
        else
        {
            throw;
        }
    }
}

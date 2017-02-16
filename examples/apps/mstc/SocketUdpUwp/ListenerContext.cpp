#include "pch.h"
#include <stdlib.h>
#include <utility>
#include <string>
#include "ListenerContext.h"

using namespace Concurrency;

SocketUdpUwp::ListenerContext::ListenerContext(
    IAsyncThreadPage^ page,
    DatagramSocket^   listener,
    String^           serverName) :
    page_{ std::move(page) }, listener_{ std::move(listener) }, serverName_{ std::move(serverName) }
{
    ::InitializeCriticalSectionEx(&lock_, 0, 0);
}

SocketUdpUwp::ListenerContext::~ListenerContext()
{
    // A DatagramSocket can be closed in two ways:
    //  - explicitly: using the 'delete' keyword (listener is closed even if there are outstanding references to it).
    //  - implicitly: removing the last reference to it (i.e., falling out-of-scope).
    //
    // When a DatagramSocket is closed implicitly, it can take several seconds for the local UDP port being used
    // by it to be freed/reclaimed by the lower networking layers. During that time, other UDP sockets on the machine
    // will not be able to use the port. Thus, it is strongly recommended that DatagramSocket instances be explicitly
    // closed before they go out of scope(e.g., before application exit). The call below explicitly closes the socket.

    if (listener_ != nullptr)
    {
        delete listener_;
        listener_ = nullptr;
    }

    ::DeleteCriticalSection(&lock_);
}

void
SocketUdpUwp::ListenerContext::OnMessage(
    DatagramSocket^           socket,
    MessageReceivedEventArgs^ eventArgs)
{
    if (outputStream_ != nullptr)
    {
        EchoMessage(eventArgs);
        return;
    }

    // We do not have an output stream yet so create one.
    create_task(socket->GetOutputStreamAsync(eventArgs->RemoteAddress, eventArgs->RemotePort)).then(
        [this, socket, eventArgs](IOutputStream^ stream)
    {
        // It might happen that the OnMessage was invoked more than once before the GetOutputStreamAsync call
        // completed. In this case we will end up with multiple streams - just keep one of them.
        EnterCriticalSection(&lock_);

        if (outputStream_ == nullptr)
        {
            outputStream_ = stream;
        }

        LeaveCriticalSection(&lock_);

        EchoMessage(eventArgs);
    }).then([this](task<void> prevTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            prevTask.get();
        }
        catch (Exception^ ex)
        {
            page_->NotifyFromAsyncThread("On message with an error: " + ex->Message,
                NotifyType::Error);
        }
        catch (task_canceled&)
        {
            // Do not print anything here - this will usually happen because user closed the client socket.
        }
    });
}

void
SocketUdpUwp::ListenerContext::EchoMessage(
    MessageReceivedEventArgs ^ eventArgs)
{
    auto dataReader = eventArgs->GetDataReader();
    auto strLen = static_cast<unsigned int>(dataReader->UnconsumedBufferLength);
    if (!strLen) { return; }

    auto msg = dataReader->ReadString(strLen);

    wchar_t buf[256];
    auto len = swprintf_s(buf, L"server%s receive \"%s\" from client",
        serverName_->IsEmpty() ? L"" : (" " + serverName_)->Data(), msg->Data());
    auto display = ref new String(buf);
    page_->NotifyFromAsyncThread(display, NotifyType::Status);

    len += swprintf_s(&buf[len], _countof(buf) - len, L" - got %d chars",
        msg->Length());
    auto echo = ref new String(buf);

    auto dataWriter = ref new DataWriter(outputStream_);

    try
    {
        dataWriter->WriteString(echo);
    }
    catch (Exception^ ex)
    {
        page_->NotifyFromAsyncThread("Echoing failed with error: " + ex->Message, NotifyType::Error);
    }

    create_task(dataWriter->StoreAsync()).then(
        [this](task<unsigned int> writeTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            writeTask.get();
        }
        catch (Exception^ ex)
        {
            page_->NotifyFromAsyncThread("Echo message with an error: " + ex->Message,
                NotifyType::Error);
        }
    });
}
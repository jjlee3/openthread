#include "pch.h"
#include <stdlib.h>
#include <utility>
#include <string>
#include "ListenerContext.h"

using namespace Concurrency;
using namespace Platform;
using namespace Windows::UI::Core;
using namespace Windows::Security::Cryptography;

SocketUwp::ListenerContext::ListenerContext(
    IAsyncThreadPage^ page,
    IAppSocket^       listener,
    String^           serverName)
    : page_{ std::move(page) }, listener_{ std::move(listener) }, serverName_{ std::move(serverName) }
{
    ::InitializeCriticalSectionEx(&lock_, 0, 0);
}

SocketUwp::ListenerContext::~ListenerContext()
{
    // A DatagramSocket can be closed in two ways:
    //  - explicitly: using the 'delete' keyword (listener is closed even if there are outstanding references to it).
    //  - implicitly: removing the last reference to it (i.e., falling out-of-scope).
    //
    // When a DatagramSocket is closed implicitly, it can take several seconds for the local UDP port being used
    // by it to be freed/reclaimed by the lower networking layers. During that time, other UDP sockets on the machine
    // will not be able to use the port. Thus, it is strongly recommended that DatagramSocket instances be explicitly
    // closed before they go out of scope(e.g., before application exit). The call below explicitly closes the socket.

    delete listener_;
    listener_ = nullptr;

    ::DeleteCriticalSection(&lock_);
}

void
SocketUwp::ListenerContext::OnMessage(
    IAppSocket^               socket,
    MessageReceivedEventArgs^ eventArgs)
{
    if (outputStream_ != nullptr)
    {
        EchoMessage(eventArgs);
        return;
    }

    // We do not have an output stream yet so create one.
    create_task(socket->GetOutputStreamAsync(eventArgs->RemoteHost, eventArgs->RemotePort)).then(
        [this, socket, eventArgs](IOutputStream^ stream)
    {
        // It might happen that the OnMessage was invoked more than once before the GetOutputStreamAsync call
        // completed. In this case we will end up with multiple streams - just keep one of them.
        EnterCriticalSection(&lock_);

        if (outputStream_ == nullptr)
        {
            socket->SetConnected();
            //outputStream_ = socket->OutputStream;
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
        catch (Exception^ e)
        {
            page_->NotifyFromAsyncThread("On message with an error: " + e->Message,
                NotifyType::Error);
        }
    });
}

void
SocketUwp::ListenerContext::EchoMessage(
    MessageReceivedEventArgs^ eventArgs)
{
    auto reader = eventArgs->GetDataReader();
    auto strLen = static_cast<unsigned int>(reader->UnconsumedBufferLength);
    if (!strLen) { return; }

    auto msg = reader->ReadString(strLen);

    wchar_t buf[256];
    auto len = swprintf_s(buf, L" server receive \"%s\" from client", msg->Data());
    auto received = ref new String(buf);
    page_->NotifyFromAsyncThread(received, NotifyType::Status);

    if (serverName_->IsEmpty())
    {
        swprintf_s(&buf[len], _countof(buf) - len, L" - server got %d chars",
            msg->Length());
    }
    else
    {
        swprintf_s(&buf[len], _countof(buf) - len, L" - server %s got %d chars",
            serverName_->Data(), msg->Length());
    }

    auto echo = ref new String(buf);

    auto buffer = CryptographicBuffer::ConvertStringToBinary(echo,
        BinaryStringEncoding::Utf16LE);

    create_task(outputStream_->WriteAsync(buffer)).then(
        [this](task<unsigned int> writeTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            writeTask.get();
        }
        catch (Exception^ e)
        {
            page_->NotifyFromAsyncThread("Echoing message with an error: " + e->Message,
                NotifyType::Error);
        }
    });
}
#include "pch.h"
#include <stdlib.h>
#include <utility>
#include "ListenerContext.h"

using namespace Concurrency;
using namespace Platform;

SocketTcpUwp::ListenerContext::ListenerContext(
    IAsyncThreadPage^     page,
    StreamSocketListener^ listener,
    String^               serverName) :
    page_{ std::move(page) }, listener_{ std::move(listener) }, serverName_ { std::move(serverName) }
{
}

SocketTcpUwp::ListenerContext::~ListenerContext()
{
    if (listener_ != nullptr)
    {
        delete listener_;
        listener_ = nullptr;
    }
}

void
SocketTcpUwp::ListenerContext::OnConnection(
    StreamSocketListener^        listener,
    ConnectionReceivedEventArgs^ args)
{
    auto dataReader = ref new DataReader(args->Socket->InputStream);
    auto dataWriter = ref new DataWriter(args->Socket->OutputStream);

    ReceiveLoop(args->Socket, dataReader, dataWriter);
}

void
SocketTcpUwp::ListenerContext::ReceiveLoop(
    StreamSocket^ socket,
    DataReader^   dataReader,
    DataWriter^   dataWriter)
{
    // Read first 4 bytes (length of the subsequent string).
    create_task(dataReader->LoadAsync(sizeof(UINT32))).then(
        [this, socket, dataReader, dataWriter](unsigned int size)
    {
        if (size < sizeof(UINT32))
        {
            // The underlying socket was closed before we were able to read the whole data.
            cancel_current_task();
        }

        unsigned int stringLength = dataReader->ReadUInt32();
        return create_task(dataReader->LoadAsync(stringLength)).then(
            [this, socket, dataReader, dataWriter, stringLength](unsigned int actualStringLength)
        {
            if (actualStringLength != stringLength)
            {
                // The underlying socket was closed before we were able to read the whole data.
                cancel_current_task();
            }

            EchoMessage(dataReader, dataWriter);
        });
    }).then([this, socket, dataReader, dataWriter](task<void> previousTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            previousTask.get();

            // Everything went ok, so try to receive another string. The receive will continue until the stream is
            // broken (i.e. peer closed the socket).
            ReceiveLoop(socket, dataReader, dataWriter);
        }
        catch (Exception^ ex)
        {
            page_->NotifyFromAsyncThread("Read stream failed with error: " + ex->Message,
                NotifyType::Error);

            // Explicitly close the socket.
            delete socket;
        }
        catch (task_canceled&)
        {
            // Do not print anything here - this will usually happen because user closed the client socket.

            // Explicitly close the socket.
            delete socket;
        }
    });
}

void
SocketTcpUwp::ListenerContext::EchoMessage(
    DataReader^ dataReader,
    DataWriter^ dataWriter)
{
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

    try
    {
        dataWriter->WriteUInt32(echo->Length());
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
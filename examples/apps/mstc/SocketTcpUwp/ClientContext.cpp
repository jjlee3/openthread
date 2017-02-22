#include "pch.h"
#include <utility>
#include "ClientContext.h"

using namespace SocketTcpUwp;

using namespace Concurrency;

ClientContext::ClientContext(
    IAsyncThreadPage^ page,
    StreamSocket^     client) :
    page_{ std::move(page) }, client_{ std::move(client) }
{
}

ClientContext::~ClientContext()
{
    if (client_ != nullptr)
    {
        delete client_;
        client_ = nullptr;
    }
}

void
ClientContext::OnConnection(
    StreamSocket^ client)
{
    connected_ = true;
    ReceiveLoop(client, GetDataReader());
}

void
ClientContext::SendMessage(
    String^ msg)
{
    if (!IsConnected())
    {
        page_->NotifyFromAsyncThread("This socket is not yet connected.", NotifyType::Error);
        return;
    }

    auto dataWriter = GetDataWriter();

    try
    {
        dataWriter->WriteUInt32(msg->Length());
        dataWriter->WriteString(msg);
        page_->NotifyFromAsyncThread("Sending - " + msg, NotifyType::Status);
    }
    catch (Exception^ ex)
    {
        page_->NotifyFromAsyncThread("Sending failed with error: " + ex->Message, NotifyType::Error);
    }

    // Write the locally buffered data to the network. Please note that write operation will succeed
    // even if the server is not listening.
    create_task(dataWriter->StoreAsync()).then(
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

DataReader^
ClientContext::GetDataReader()
{
    if (dataReader_ == nullptr)
    {
        dataReader_ = ref new DataReader(client_->InputStream);
    }

    return dataReader_;
}

DataWriter^
ClientContext::GetDataWriter()
{
    if (dataWriter_ == nullptr)
    {
        dataWriter_ = ref new DataWriter(client_->OutputStream);
    }

    return dataWriter_;
}

void
ClientContext::ReceiveLoop(
    StreamSocket^ socket,
    DataReader^   dataReader)
{
    // Read first 4 bytes (length of the subsequent string).
    create_task(dataReader->LoadAsync(sizeof(UINT32))).then(
        [this, socket, dataReader](unsigned int size)
    {
        if (size < sizeof(UINT32))
        {
            // The underlying socket was closed before we were able to read the whole data.
            cancel_current_task();
        }

        unsigned int stringLength = dataReader->ReadUInt32();
        return create_task(dataReader->LoadAsync(stringLength)).then(
            [this, socket, dataReader, stringLength](unsigned int actualStringLength)
        {
            if (actualStringLength != stringLength)
            {
                // The underlying socket was closed before we were able to read the whole data.
                cancel_current_task();
            }

            auto msg = dataReader->ReadString(actualStringLength);

            page_->NotifyFromAsyncThread(
                "Received data from server peer: \"" + msg + "\"",
                NotifyType::Status);
        });
    }).then([this, socket, dataReader](task<void> previousTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            previousTask.get();

            // Everything went ok, so try to receive another string. The receive will continue until the stream is
            // broken (i.e. peer closed the socket).
            ReceiveLoop(socket, dataReader);
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

#include "pch.h"
#include <utility>
#include "ClientContextHelper.h"

using namespace OpenThreadTalk;

using namespace Concurrency;

ClientContextHelper::ClientContextHelper(
    IAsyncThreadPage^ page) :
    ContextHelper{ std::move(page) }
{
}

void
ClientContextHelper::SendMessage(
    DataWriter^ dataWriter,
    bool        withMsgLen,
    String^     msg)
{
    if (!IsConnected())
    {
        NotifyFromAsyncThread("This socket is not yet connected.", NotifyType::Error);
        return;
    }

    try
    {
        if (withMsgLen) { dataWriter->WriteUInt32(msg->Length()); }
        dataWriter->WriteString(msg);
        NotifyFromAsyncThread("Sending - " + msg, NotifyType::Status);
    }
    catch (Exception^ ex)
    {
        NotifyFromAsyncThread("Sending failed with error: " + ex->Message, NotifyType::Error);
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
            NotifyFromAsyncThread("Send failed with error: " + ex->Message, NotifyType::Error);
        }
    });
}

void
ClientContextHelper::Receive(
    DataReader^  dataReader,
    unsigned int strLen)
{
    if (!strLen) { return; }

    auto msg = dataReader->ReadString(strLen);
    NotifyFromAsyncThread("Received data from server: \"" + msg + "\"",
        NotifyType::Status);
}

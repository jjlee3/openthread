#include "pch.h"
#include <stdlib.h>
#include <utility>
#include "ListenerContextHelper.h"

using namespace Concurrency;

SocketUwp::ListenerContextHelper::ListenerContextHelper(
    IAsyncThreadPage^ page,
    String^           serverName) :
    ContextHelper{ std::move(page) }, serverName_ { std::move(serverName) }
{
}

void
SocketUwp::ListenerContextHelper::Receive(
    DataReader^           dataReader,
    unsigned int          strLen,
    DataWriter^           dataWriter,
    bool                  withMsgLen)
{
    if (!strLen) { return; }

    auto msg = dataReader->ReadString(strLen);
    NotifyFromAsyncThread("Received data from client: \"" + msg + "\"",
        NotifyType::Status);
    auto echo = CreateEchoMessage(msg);
    EchoMessage(dataWriter, withMsgLen, echo);
}

SocketUwp::String^
SocketUwp::ListenerContextHelper::CreateEchoMessage(
    String^ msg)
{
    wchar_t buf[256];
    auto len = swprintf_s(buf, L"Server%s received data from client : \"%s\"",
        serverName_->IsEmpty() ? L"" : (" " + serverName_)->Data(), msg->Data());

    len += swprintf_s(&buf[len], _countof(buf) - len, L" - got %d chars",
        msg->Length());
    return ref new String(buf);
}

void
SocketUwp::ListenerContextHelper::EchoMessage(
    DataWriter^ dataWriter,
    bool        withMsgLen,
    String^     echo)
{
    try
    {
        if (withMsgLen) { dataWriter->WriteUInt32(echo->Length()); }
        dataWriter->WriteString(echo);
    }
    catch (Exception^ ex)
    {
        NotifyFromAsyncThread("Echoing failed with error: " + ex->Message, NotifyType::Error);
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
            NotifyFromAsyncThread("Echo message with an error: " + ex->Message,
                NotifyType::Error);
        }
    });
}

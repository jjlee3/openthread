#include "pch.h"
#include "StreamClientContext.h"

using namespace OpenThreadTalk;

using namespace Concurrency;

StreamClientContext::StreamClientContext(
    IAsyncThreadPage^ page,
    StreamSocket^     client,
    ClientArgs^       args) :
    client_{ std::move(client) },
    args_{ std::move(args) },
    helper_{ std::move(page) }
{
}

StreamClientContext::~StreamClientContext()
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
StreamClientContext::Connect_Click(
    Object^          sender,
    RoutedEventArgs^ e)
{
    task<void> removeContext;

    if (CoreApplication::Properties->HasKey("clientContext"))
    {
        auto clientContext = dynamic_cast<IClientContext^>(
            CoreApplication::Properties->Lookup("clientContext"));
        if (clientContext == nullptr)
        {
            throw ref new FailureException(L"No clientContext");
        }

        removeContext = create_task(clientContext->CancelIO()).then(
            []()
        {
            CoreApplication::Properties->Remove("clientContext");
        });
    }
    else
    {
        removeContext = create_task([]() {});
    }

    removeContext.then([this](task<void> prevTask)
    {
        try
        {
            // Try getting an exception.
            prevTask.get();

            // Events cannot be hooked up directly to the ScenarioInput2 object, as the object can fall out-of-scope and be
            // deleted. This would render any event hooked up to the object ineffective. The ClientContext guarantees that
            // both the socket and object that serves its events have the same lifetime.
            CoreApplication::Properties->Insert("clientContext", this);
        }
        catch (Exception^ ex)
        {
            helper_.NotifyFromAsyncThread(
                "Remove clientContext error: " + ex->Message,
                NotifyType::Error);
        }
        catch (task_canceled&)
        {
        }
    }).then([this]()
    {
        auto endpointPair = ref new EndpointPair(args_->ClientHostName, args_->ClientPort,
            args_->ServerHostName, args_->ServerPort);

        create_task(client_->ConnectAsync(endpointPair)).then(
            [this, endpointPair](task<void> prevTask)
        {
            try
            {
                // Try getting an exception.
                prevTask.get();
                helper_.NotifyFromAsyncThread(
                    "Connect from " + endpointPair->LocalHostName->CanonicalName +
                    " to " + endpointPair->RemoteHostName->CanonicalName,
                    NotifyType::Status);
                OnConnection(client_);
            }
            catch (Exception^ ex)
            {
                CoreApplication::Properties->Remove("clientContext");
                helper_.NotifyFromAsyncThread(
                    "Start binding failed with error: " + ex->Message,
                    NotifyType::Error);
            }
            catch (task_canceled&)
            {
                CoreApplication::Properties->Remove("clientContext");
            }
        });
    });
}

void
StreamClientContext::Send_Click(
    Object^          sender,
    RoutedEventArgs^ e,
    String^          input)
{
    helper_.SendMessage(GetDataWriter(), true, input);
}

IAsyncAction^
StreamClientContext::CancelIO()
{
    return client_->CancelIOAsync();
}

void
StreamClientContext::OnConnection(
    StreamSocket^ streamSocket)
{
    helper_.SetConnected(true);
    helper_.ReceiveLoop(streamSocket, GetDataReader());
}

DataReader^
StreamClientContext::GetDataReader()
{
    if (dataReader_ == nullptr)
    {
        dataReader_ = ref new DataReader(client_->InputStream);
    }

    return dataReader_;
}

DataWriter^
StreamClientContext::GetDataWriter()
{
    if (dataWriter_ == nullptr)
    {
        dataWriter_ = ref new DataWriter(client_->OutputStream);
    }

    return dataWriter_;
}

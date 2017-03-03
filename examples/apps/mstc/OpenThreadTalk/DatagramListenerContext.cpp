#include "pch.h"
#include <utility>
#include "DatagramListenerContext.h"

using namespace OpenThreadTalk;

using namespace Concurrency;

DatagramListenerContext::DatagramListenerContext(
    IAsyncThreadPage^ page,
    DatagramSocket^   listener,
    ListenerArgs^     args) :
    listener_{ std::move(listener) },
    args_ { std::move(args) },
    helper_{ std::move(page), args_->ServerName }
{
    ::InitializeCriticalSectionEx(&lock_, 0, 0);
}

DatagramListenerContext::~DatagramListenerContext()
{
    // A Listener can be closed in two ways:
    //  - explicitly: using the 'delete' keyword (listener is closed even if there are outstanding references to it).
    //  - implicitly: removing the last reference to it (i.e., falling out-of-scope).
    //
    // When a Socket is closed implicitly, it can take several seconds for the local port being used
    // by it to be freed/reclaimed by the lower networking layers. During that time, other sockets on the machine
    // will not be able to use the port. Thus, it is strongly recommended that Socket instances be explicitly
    // closed before they go out of scope(e.g., before application exit). The call below explicitly closes the socket.
    if (listener_ != nullptr)
    {
        delete listener_;
        listener_ = nullptr;
    }

    ::DeleteCriticalSection(&lock_);
}

void
DatagramListenerContext::Listen_Click(
    Object^          sender,
    RoutedEventArgs^ e)
{
    task<void> removeContext;

    if (CoreApplication::Properties->HasKey("listenerContext"))
    {
        auto listenerContext = dynamic_cast<IListenerContext^>(
            CoreApplication::Properties->Lookup("listenerContext"));
        if (listenerContext == nullptr)
        {
            throw ref new FailureException(L"No listenerContext");
        }

        removeContext = create_task(listenerContext->CancelIO()).then(
            []()
        {
            CoreApplication::Properties->Remove("listenerContext");
        });
    }

    listener_->MessageReceived += ref new MessageHandler(
        this, &DatagramListenerContext::OnMessage);

    removeContext.then([this](task<void> prevTask)
    {
        try
        {
            // Try getting an exception.
            prevTask.get();

            // Events cannot be hooked up directly to the ScenarioInput1 object, as the object can fall out-of-scope and be
            // deleted. This would render any event hooked up to the object ineffective. The ListenerContext guarantees that
            // both the listener and object that serves its events have the same lifetime.
            CoreApplication::Properties->Insert("listenerContext", this);
        }
        catch (Exception^ ex)
        {
            helper_.NotifyFromAsyncThread(
                "Remove listenerContext error: " + ex->Message,
                NotifyType::Error);
        }
        catch (task_canceled&)
        {
        }
    }).then([this]()
    {
        create_task(listener_->BindEndpointAsync(args_->ServerHostName, args_->ServerPort)).then(
            [this](task<void> prevTask)
        {
            try
            {
                // Try getting an exception.
                prevTask.get();
                helper_.NotifyFromAsyncThread(
                    "Listening on address " + args_->ServerHostName->CanonicalName,
                    NotifyType::Status);
            }
            catch (Exception^ ex)
            {
                CoreApplication::Properties->Remove("listenerContext");
                helper_.NotifyFromAsyncThread(
                    "Start listening failed with error: " + ex->Message,
                    NotifyType::Error);
            }
        });
    });
}

IAsyncAction^
DatagramListenerContext::CancelIO()
{
    return listener_->CancelIOAsync();
}

void
DatagramListenerContext::OnMessage(
    DatagramSocket^           socket,
    MessageReceivedEventArgs^ eventArgs)
{
    if (outputStream_ != nullptr)
    {
        auto dataReader = eventArgs->GetDataReader();
        helper_.Receive(dataReader, dataReader->UnconsumedBufferLength, GetDataWriter(), false);
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

        auto dataReader = eventArgs->GetDataReader();
        helper_.Receive(dataReader, dataReader->UnconsumedBufferLength, GetDataWriter(), false);
    }).then([this](task<void> prevTask)
    {
        try
        {
            // Try getting all exceptions from the continuation chain above this point.
            prevTask.get();
        }
        catch (Exception^ ex)
        {
            helper_.NotifyFromAsyncThread("On message with an error: " + ex->Message,
                NotifyType::Error);
        }
        catch (task_canceled&)
        {
            // Do not print anything here - this will usually happen because user closed the client socket.
        }
    });
}

DataWriter^
DatagramListenerContext::GetDataWriter()
{
    if (dataWriter_ == nullptr)
    {
        dataWriter_ = ref new DataWriter(outputStream_);
    }

    return dataWriter_;
}

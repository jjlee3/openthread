#include "pch.h"
#include <utility>
#include "StreamListenerContext.h"

using namespace SocketUwp;

using namespace Concurrency;

StreamListenerContext::StreamListenerContext(
    IAsyncThreadPage^     page,
    StreamSocketListener^ listener,
    ListenerArgs^         args) :
    listener_{ std::move(listener) },
    args_{ std::move(args) },
    helper_{ std::move(page), args_->ServerName }
{
}

StreamListenerContext::~StreamListenerContext()
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
}

void
StreamListenerContext::Listen_Click(
    Object^          sender,
    RoutedEventArgs^ e)
{
    if (CoreApplication::Properties->HasKey("listenerContext"))
    {
        CoreApplication::Properties->Remove("listenerContext");
    }

    listener_->ConnectionReceived += ref new ConnectionHandler(
        this, &StreamListenerContext::OnConnection);

    // Events cannot be hooked up directly to the ScenarioInput1 object, as the object can fall out-of-scope and be
    // deleted. This would render any event hooked up to the object ineffective. The ListenerContext guarantees that
    // both the listener and object that serves its events have the same lifetime.
    CoreApplication::Properties->Insert("listenerContext", this);

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
}

void
StreamListenerContext::OnConnection(
    StreamSocketListener^        listener,
    ConnectionReceivedEventArgs^ args)
{
    auto dataReader = ref new DataReader(args->Socket->InputStream);
    auto dataWriter = ref new DataWriter(args->Socket->OutputStream);

    helper_.ReceiveLoop(args->Socket, dataReader, dataWriter);
}

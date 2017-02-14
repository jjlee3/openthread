//
// ServerControl.xaml.cpp
// Implementation of the ServerControl class
//

#include "pch.h"
#include "ServerControl.xaml.h"
#include "Types.h"
#include "ListenerContext.h"

using namespace Concurrency;
using namespace Platform;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

SocketTcpUwp::ServerControl::ServerControl()
{
	InitializeComponent();
    ServerPort->Text = DEF_PORT.ToString();
}

void
SocketTcpUwp::ServerControl::Init(
    IAsyncThreadPage^ page)
{
    page_ = page;
}

void
SocketTcpUwp::ServerControl::Listen_Click(
    Platform::Object^                   sender,
    Windows::UI::Xaml::RoutedEventArgs^ e)
{
    try
    {
        auto serverName = ServerName->Text;

        auto serverIP = ServerIP->Text;
        if (serverIP->IsEmpty())
        {
            throw ref new InvalidArgumentException(L"No Server IP");
        }

        auto serverHostName = ref new HostName(serverIP);

        Service^ serverPort = ServerPort->Text;
        if (serverPort->IsEmpty())
        {
            serverPort = DEF_PORT.ToString();
        }

        using CoreApplication = Windows::ApplicationModel::Core::CoreApplication;
        if (CoreApplication::Properties->HasKey("listenerContext"))
        {
            CoreApplication::Properties->Remove("listenerContext");
        }

        auto listener = ref new StreamSocketListener();
        auto listenerContext = ref new ListenerContext(page_, listener, serverName);

        listener->ConnectionReceived += ref new ConnectionHandler(
            listenerContext, &ListenerContext::OnConnection);

        // Events cannot be hooked up directly to the ScenarioInput1 object, as the object can fall out-of-scope and be
        // deleted. This would render any event hooked up to the object ineffective. The ListenerContext guarantees that
        // both the listener and object that serves its events have the same lifetime.
        CoreApplication::Properties->Insert("listenerContext", listenerContext);

        create_task(listener->BindEndpointAsync(serverHostName, serverPort)).then(
            [this, serverHostName](task<void> prevTask)
        {
            try
            {
                // Try getting an exception.
                prevTask.get();
                page_->NotifyFromAsyncThread(
                    "Listening on address " + serverHostName->CanonicalName,
                    NotifyType::Status);
            }
            catch (Exception^ ex)
            {
                CoreApplication::Properties->Remove("listenerContext");
                page_->NotifyFromAsyncThread(
                    "Start listening failed with error: " + ex->Message,
                    NotifyType::Error);
            }
        });
    }
    catch (Exception^ ex)
    {
        page_->NotifyFromAsyncThread(
            "Listening failed with input error: " + ex->Message,
            NotifyType::Error);
    }
}
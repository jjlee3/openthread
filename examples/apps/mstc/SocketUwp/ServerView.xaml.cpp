//
// ServerView.xaml.cpp
// Implementation of the ServerView class
//

#include "pch.h"
#include "ServerView.xaml.h"
#include "AppDatagramSocket.h"
#include "ListenerContext.h"

using namespace Concurrency;
using namespace Platform;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

SocketUwp::ServerView::ServerView()
{
	InitializeComponent();
    ServerPort->Text = DEF_PORT.ToString();
}

void
SocketUwp::ServerView::Init(
    IAsyncThreadPage^ page)
{
    page_ = page;
}


void
SocketUwp::ServerView::Listen_Click(
    Platform::Object^                   sender,
    Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto serverName = ServerName->Text;

    auto serverIP = ServerIP->Text;
    if (serverIP->IsEmpty())
    {
        throw ref new InvalidArgumentException(L"No Server IP");
    }

    auto serverHostName = ref new HostName(serverIP);

    auto serverPort = ServerPort->Text;
    if (serverPort->IsEmpty())
    {
        serverPort = DEF_PORT.ToString();
    }

    using CoreApplication = Windows::ApplicationModel::Core::CoreApplication;
    if (CoreApplication::Properties->HasKey("listenerContext"))
    {
        CoreApplication::Properties->Remove("listenerContext");
    }
    
    IAppSocket^ listener = ref new AppDatagramSocket();
    auto listenerContext = ref new ListenerContext(page_, listener, serverName);

    using MessageHandler = IAppSocket::MessageHandler;
    listener->MessageReceived += ref new MessageHandler(
        listenerContext, &ListenerContext::OnMessage);

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
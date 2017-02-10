//
// ClientView.xaml.cpp
// Implementation of the ClientView class
//

#include "pch.h"
#include "ClientView.xaml.h"
#include "AppDatagramSocket.h"
#include "ClientContext.h"

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Networking;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

SocketUwp::ClientView::ClientView()
{
	InitializeComponent();
    ServerPort->Text = DEF_SERVER_PORT.ToString();
    ClientPort->Text = DEF_PORT.ToString();
}

void
SocketUwp::ClientView::Init(
    IAsyncThreadPage^ page)
{
    page_ = page;
}

void
SocketUwp::ClientView::Connect_Click(
    Platform::Object^                   sender,
    Windows::UI::Xaml::RoutedEventArgs^ e)
{
    try
    {
        auto serverIP = ServerIP->Text;
        if (serverIP->IsEmpty())
        {
            throw ref new InvalidArgumentException(L"No Server IP");
        }

        auto serverHostName = ref new HostName(serverIP);

        auto serverPort = ServerPort->Text;
        if (serverPort->IsEmpty())
        {
            serverPort = DEF_SERVER_PORT.ToString();
        }

        auto clientIP = ClientIP->Text;
        if (clientIP->IsEmpty())
        {
            throw ref new InvalidArgumentException(L"No Cient IP");
        }

        auto clientHostName = ref new HostName(clientIP);

        auto clientPort = ClientPort->Text;
        if (clientPort->IsEmpty())
        {
            clientPort = DEF_PORT.ToString();
        }

        using CoreApplication = Windows::ApplicationModel::Core::CoreApplication;
        if (CoreApplication::Properties->HasKey("clientContext"))
        {
            CoreApplication::Properties->Remove("clientContext");
        }

        IAppSocket^ client = ref new AppDatagramSocket();
        auto clientContext = ref new ClientContext(page_, client);

        using MessageHandler = IAppSocket::MessageHandler;
        client->MessageReceived += ref new MessageHandler(
            clientContext, &ClientContext::OnMessage);

        // Events cannot be hooked up directly to the ScenarioInput2 object, as the object can fall out-of-scope and be
        // deleted. This would render any event hooked up to the object ineffective. The ClientContext guarantees that
        // both the socket and object that serves its events have the same lifetime.
        CoreApplication::Properties->Insert("clientContext", clientContext);

        auto endpointPair = ref new EndpointPair(clientHostName, clientPort,
            serverHostName, serverPort);

        create_task(client->ConnectAsync(endpointPair)).then(
            [this, client, endpointPair](task<void> prevTask)
        {
            try
            {
                // Try getting an exception.
                prevTask.get();
                page_->NotifyFromAsyncThread(
                    "Connect from " + endpointPair->LocalHostName->CanonicalName +
                    " to " + endpointPair->RemoteHostName->CanonicalName,
                    NotifyType::Status);
                client->SetConnected();
            }
            catch (Exception^ ex)
            {
                CoreApplication::Properties->Remove("clientContext");
                page_->NotifyFromAsyncThread(
                    "Start binding failed with error: " + ex->Message,
                    NotifyType::Error);
            }
        });
    }
    catch (Exception^ ex)
    {
        page_->NotifyFromAsyncThread(
            "Connecting failed with input error: " + ex->Message,
            NotifyType::Error);
    }
}

void
SocketUwp::ClientView::Send_Click(
    Platform::Object^                   sender,
    Windows::UI::Xaml::RoutedEventArgs^ e)
{
    try
    {
        auto input = Input->Text;
        if (input->IsEmpty())
        {
            throw ref new InvalidArgumentException(L"No Input");
        }

        using CoreApplication = Windows::ApplicationModel::Core::CoreApplication;
        if (!CoreApplication::Properties->HasKey("clientContext"))
        {
            throw ref new FailureException(L"Not Connected");
        }

        auto clientContext = dynamic_cast<ClientContext^>(
            CoreApplication::Properties->Lookup("clientContext"));
        if (clientContext == nullptr)
        {
            throw ref new FailureException(L"No clientContext");
        }

        clientContext->SendMessage(input);
    }
    catch (Exception^ ex)
    {
        page_->NotifyFromAsyncThread(
            "Sending message failed with error: " + ex->Message,
            NotifyType::Error);
    }
}
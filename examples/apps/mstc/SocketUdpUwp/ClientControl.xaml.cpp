//
// ClientControl.xaml.cpp
// Implementation of the ClientControl class
//

#include "pch.h"
#include "ClientControl.xaml.h"
#include "ClientContext.h"
#include "Ipv6.h"

using namespace Concurrency;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

SocketUdpUwp::ClientControl::ClientControl()
{
	InitializeComponent();
    ServerPort->Text = DEF_SERVER_PORT.ToString();
    ClientPort->Text = DEF_PORT.ToString();
}

void
SocketUdpUwp::ClientControl::Init(
    IAsyncThreadPage^ page)
{
    page_ = page;
}

void
SocketUdpUwp::ClientControl::Connect_Click(
    Object^          sender,
    RoutedEventArgs^ e)
{
    try
    {
        auto serverIP = ServerIP->Text;
        if (serverIP->IsEmpty())
        {
            throw ref new InvalidArgumentException(L"No Server IP");
        }

        if (Ipv6::IsValidAddress(serverIP))
        {
        }
        else
        {
            throw ref new InvalidArgumentException(L"Not a valid Server IPv6 address");
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

        if (Ipv6::IsValidAddress(clientIP))
        {
        }
        else
        {
            throw ref new InvalidArgumentException(L"Not a valid Client IPv6 address");
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

        auto client = ref new DatagramSocket();
        auto clientContext = ref new ClientContext(page_, client);

        client->MessageReceived += ref new MessageHandler(
            clientContext, &ClientContext::OnMessage);

        // Events cannot be hooked up directly to the ScenarioInput2 object, as the object can fall out-of-scope and be
        // deleted. This would render any event hooked up to the object ineffective. The ClientContext guarantees that
        // both the socket and object that serves its events have the same lifetime.
        CoreApplication::Properties->Insert("clientContext", clientContext);

        auto endpointPair = ref new EndpointPair(clientHostName, clientPort,
            serverHostName, serverPort);

        create_task(client->ConnectAsync(endpointPair)).then(
            [this, clientContext, endpointPair](task<void> prevTask)
        {
            try
            {
                // Try getting an exception.
                prevTask.get();
                page_->NotifyFromAsyncThread(
                    "Connect from " + endpointPair->LocalHostName->CanonicalName +
                    " to " + endpointPair->RemoteHostName->CanonicalName,
                    NotifyType::Status);
                clientContext->SetConnected();
            }
            catch (Exception^ ex)
            {
                CoreApplication::Properties->Remove("clientContext");
                page_->NotifyFromAsyncThread(
                    "Start binding failed with error: " + ex->Message,
                    NotifyType::Error);
            }
            catch (task_canceled&)
            {
                CoreApplication::Properties->Remove("clientContext");
            }
        });
    }
    catch (Exception^ ex)
    {
        page_->NotifyFromAsyncThread(
            "Connecting failed with input error: " + ex->Message,
            NotifyType::Error);
    }
    catch (task_canceled&)
    {
        CoreApplication::Properties->Remove("clientContext");
    }
}

void
SocketUdpUwp::ClientControl::Send_Click(
    Object^          sender,
    RoutedEventArgs^ e)
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
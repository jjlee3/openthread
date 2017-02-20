//
// ClientControl.xaml.cpp
// Implementation of the ClientControl class
//

#include "pch.h"
#include "ClientControl.xaml.h"
#include "Factory.h"
#include "Ipv6.h"
#include "ClientContextArgs.h"

using namespace Concurrency;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

SocketUwp::ClientControl::ClientControl()
{
    InitializeComponent();
    ServerPort->Text = DEF_SERVER_PORT.ToString();
    ClientPort->Text = DEF_PORT.ToString();
}

void
SocketUwp::ClientControl::Init(
    IAsyncThreadPage^ page)
{
    page_ = page;
}

void
SocketUwp::ClientControl::Connect_Click(
    Object^          sender,
    RoutedEventArgs^ e)
{
    try
    {
        auto clientContextArgs = ref new ClientContextArgs();

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

        clientContextArgs->ServerHostName = ref new HostName(serverIP);

        clientContextArgs->ServerPort = ServerPort->Text;
        if (clientContextArgs->ServerPort->IsEmpty())
        {
            clientContextArgs->ServerPort = DEF_SERVER_PORT.ToString();
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

        clientContextArgs->ClientHostName = ref new HostName(clientIP);

        clientContextArgs->ClientPort = ClientPort->Text;
        if (clientContextArgs->ClientPort->IsEmpty())
        {
            clientContextArgs->ClientPort = DEF_PORT.ToString();
        }

        auto cleintContext = Factory::CreateClientContext(page_, clientContextArgs);
        cleintContext->Connect_Click(sender, e);
    }
    catch (Exception^ ex)
    {
        CoreApplication::Properties->Remove("clientContext");
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
SocketUwp::ClientControl::Send_Click(
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

        if (!CoreApplication::Properties->HasKey("clientContext"))
        {
            throw ref new FailureException(L"Not Connected");
        }

        auto clientContext = dynamic_cast<IClientContext^>(
            CoreApplication::Properties->Lookup("clientContext"));
        if (clientContext == nullptr)
        {
            throw ref new FailureException(L"No clientContext");
        }

        clientContext->Send_Click(sender, e, input);
    }
    catch (Exception^ ex)
    {
        page_->NotifyFromAsyncThread(
            "Sending message failed with error: " + ex->Message,
            NotifyType::Error);
    }
}

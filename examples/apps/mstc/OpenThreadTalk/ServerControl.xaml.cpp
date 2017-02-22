//
// ServerControl.xaml.cpp
// Implementation of the ServerControl class
//

#include "pch.h"
#include "ServerControl.xaml.h"
#include "ListenerArgs.h"
#include "Ipv6.h"
#include "Factory.h"

using namespace OpenThreadTalk;

using namespace Concurrency;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

ServerControl::ServerControl()
{
	InitializeComponent();

    ServerPort->Text = DEF_PORT.ToString();
}

void
ServerControl::Init(
    IAsyncThreadPage^ page)
{
    page_ = page;
}

void
ServerControl::Listen_Click(
    Object^          sender,
    RoutedEventArgs^ e)
{
    try
    {
        auto listenerArgs = ref new ListenerArgs();

        listenerArgs->ServerName = ServerName->Text;

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

        listenerArgs->ServerHostName = ref new HostName(serverIP);

        listenerArgs->ServerPort = ServerPort->Text;
        if (listenerArgs->ServerPort->IsEmpty())
        {
            listenerArgs->ServerPort = DEF_PORT.ToString();
        }

        auto listenerContext = Factory::CreateListenerContext(page_, listenerArgs);
        listenerContext->Listen_Click(sender, e);
    }
    catch (Exception^ ex)
    {
        page_->NotifyFromAsyncThread(
            "Listening failed with input error: " + ex->Message,
            NotifyType::Error);
    }
}

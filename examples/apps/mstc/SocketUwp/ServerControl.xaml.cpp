//
// ServerControl.xaml.cpp
// Implementation of the ServerControl class
//

#include "pch.h"
#include "ServerControl.xaml.h"
#include "Ipv6.h"
#include "ListenerContextArgs.h"
#include "Factory.h"

using namespace SocketUwp;

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
        auto listenerContextArgs = ref new ListenerContextArgs();

        listenerContextArgs->ServerName = ServerName->Text;

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

        listenerContextArgs->ServerHostName = ref new HostName(serverIP);

        listenerContextArgs->ServerPort = ServerPort->Text;
        if (listenerContextArgs->ServerPort->IsEmpty())
        {
            listenerContextArgs->ServerPort = DEF_PORT.ToString();
        }

        auto listenerContext = Factory::CreateListenerContext(page_, listenerContextArgs);
        listenerContext->Listen_Click(sender, e);
    }
    catch (Exception^ ex)
    {
        page_->NotifyFromAsyncThread(
            "Listening failed with input error: " + ex->Message,
            NotifyType::Error);
    }
}

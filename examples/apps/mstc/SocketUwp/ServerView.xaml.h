//
// ServerView.xaml.h
// Declaration of the ServerView class
//

#pragma once

#include <WinSock2.h>
#include <ws2ipdef.h>
#include <mstcpip.h>
#include "ServerView.g.h"
#include "IAsyncThreadPage.h"
#include "Consts.h"

namespace SocketUwp
{
    namespace WFM = Windows::Foundation::Metadata;

    [WFM::WebHostHidden]
    public ref class ServerView sealed
	{
	public:
        using HostName = Windows::Networking::HostName;
        using String = Platform::String;
        using Service = String;

        property static unsigned short DEF_PORT
        {
            unsigned short get() { return Consts::DEF_SERVER_PORT; }
        }

        ServerView();

        void Init(IAsyncThreadPage^ page);

    private:
        void Listen_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        IAsyncThreadPage^ page_;
    };
}

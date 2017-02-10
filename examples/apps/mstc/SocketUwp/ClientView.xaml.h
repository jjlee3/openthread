//
// ClientView.xaml.h
// Declaration of the ClientView class
//

#pragma once

#include "ClientView.g.h"
#include "IAsyncThreadPage.h"
#include "Consts.h"

namespace SocketUwp
{
    namespace WFM = Windows::Foundation::Metadata;

    [WFM::WebHostHidden]
    public ref class ClientView sealed
	{
	public:
        using HostName = Windows::Networking::HostName;
        using String = Platform::String;
        using Service = String;

        property static unsigned short DEF_SERVER_PORT
        {
            unsigned short get() { return Consts::DEF_SERVER_PORT; }
        }

        property static unsigned short DEF_PORT
        {
            unsigned short get() { return Consts::DEF_CLIENT_PORT; }
        }

        ClientView();

        void Init(IAsyncThreadPage^ page);

    private:
        void Connect_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void Send_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        IAsyncThreadPage^ page_;
    };
}

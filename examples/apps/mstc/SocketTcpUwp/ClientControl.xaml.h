//
// ClientControl.xaml.h
// Declaration of the ClientControl class
//

#pragma once

#include "ClientControl.g.h"
#include "Consts.h"
#include "IAsyncThreadPage.h"

namespace SocketTcpUwp
{
    namespace WFM = Windows::Foundation::Metadata;

    [WFM::WebHostHidden]
    public ref class ClientControl sealed
	{
	public:
        property static unsigned short DEF_SERVER_PORT
        {
            unsigned short get() { return Consts::DEF_SERVER_PORT; }
        }

        property static unsigned short DEF_PORT
        {
            unsigned short get() { return Consts::DEF_CLIENT_PORT; }
        }

        ClientControl();

        void Init(IAsyncThreadPage^ page);

    private:
        void Connect_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void Send_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        IAsyncThreadPage^ page_;
    };
}

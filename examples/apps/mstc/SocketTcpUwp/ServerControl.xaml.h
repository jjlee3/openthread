//
// ServerControl.xaml.h
// Declaration of the ServerControl class
//

#pragma once

#include "ServerControl.g.h"
#include "Consts.h"
#include "IAsyncThreadPage.h"

namespace SocketTcpUwp
{
    namespace WFM = Windows::Foundation::Metadata;

    [WFM::WebHostHidden]
    public ref class ServerControl sealed
	{
    public:
        property static unsigned short DEF_PORT
        {
            unsigned short get() { return Consts::DEF_SERVER_PORT; }
        }

        ServerControl();

        void Init(IAsyncThreadPage^ page);

    private:
        void Listen_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        IAsyncThreadPage^ page_;
    };
}

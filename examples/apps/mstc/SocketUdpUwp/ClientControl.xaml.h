//
// ClientControl.xaml.h
// Declaration of the ClientControl class
//

#pragma once

#include "ClientControl.g.h"
#include "Constsh.h"
#include "Types.h"
#include "IAsyncThreadPage.h"

namespace SocketUdpUwp
{
    namespace WFM = Windows::Foundation::Metadata;

    [WFM::WebHostHidden]
    public ref class ClientControl sealed
	{
	public:
		ClientControl();

        void Init(IAsyncThreadPage^ page);

    private:
        static constexpr unsigned short DEF_PORT = Consts::DEF_CLIENT_PORT;
        static constexpr unsigned short DEF_SERVER_PORT = Consts::DEF_SERVER_PORT;

        void Connect_Click(Object^ sender, RoutedEventArgs^ e);

        void Send_Click(Object^ sender, RoutedEventArgs^ e);


        IAsyncThreadPage^ page_;
    };
}

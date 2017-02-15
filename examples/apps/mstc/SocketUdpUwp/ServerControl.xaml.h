//
// ServerControl.xaml.h
// Declaration of the ServerControl class
//

#pragma once

#include "ServerControl.g.h"
#include "Constsh.h"
#include "Types.h"
#include "IAsyncThreadPage.h"

namespace SocketUdpUwp
{
    namespace WFM = Windows::Foundation::Metadata;

    [WFM::WebHostHidden]
    public ref class ServerControl sealed
	{
	public:
		ServerControl();

        void Init(IAsyncThreadPage^ page);

    private:
        static constexpr unsigned short DEF_PORT = Consts::DEF_SERVER_PORT;

        void Listen_Click(Object^ sender, RoutedEventArgs^ e);

        IAsyncThreadPage^ page_;
    };
}

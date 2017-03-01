//
// ClientControl.xaml.h
// Declaration of the ClientControl class
//

#pragma once

#include "ClientControl.g.h"
#include "Consts.h"
#include "Types.h"
#include "IAsyncThreadPage.h"
#include "IMainPageUIElements.h"

namespace OpenThreadTalk
{
    [WFM::WebHostHidden]
    public ref class ClientControl sealed
    {
    public:
        ClientControl();

        void Init(IAsyncThreadPage^ page, IMainPageUIElements^ mainPageUIElements);

    private:
        static constexpr unsigned short DEF_PORT = Consts::DEF_CLIENT_PORT;
        static constexpr unsigned short DEF_SERVER_PORT = Consts::DEF_SERVER_PORT;

        void Connect_Click(Object^ sender, RoutedEventArgs^ e);

        void Send_Click(Object^ sender, RoutedEventArgs^ e);

        void Exit_Click(Object^ sender, RoutedEventArgs^ e);

        IAsyncThreadPage^    page_;
        IMainPageUIElements^ mainPageUIElements_;
    };
}

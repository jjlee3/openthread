//
// ServerControl.xaml.h
// Declaration of the ServerControl class
//

#pragma once

#include "ServerControl.g.h"
#include "Consts.h"
#include "Types.h"
#include "IAsyncThreadPage.h"
#include "IMainPageUIElements.h"

namespace OpenThreadTalk
{
    [WFM::WebHostHidden]
    public ref class ServerControl sealed
    {
    public:
        ServerControl();

        void Init(IAsyncThreadPage^ page, IMainPageUIElements^ mainPageUIElements);

    private:
        static constexpr unsigned short DEF_PORT = Consts::DEF_SERVER_PORT;

        void Listen_Click(Object^ sender, RoutedEventArgs^ e);

        void Exit_Click(Object^ sender, RoutedEventArgs^ e);

        IAsyncThreadPage^    page_;
        IMainPageUIElements^ mainPageUIElements_;
    };
}

﻿//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "Types.h"
#include "IAsyncThreadPage.h"

namespace SocketUwp
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public ref class MainPage sealed : public IAsyncThreadPage
    {
    public:
        MainPage();

        virtual void NotifyFromAsyncThread(String^ message, NotifyType type);

        void Notify(String^ message, NotifyType type);

    private:
        // (row, col) position of MainPageGrid for (server or client) Control
        static constexpr int ROLE_POS_ROW = 4;
        static constexpr int ROLE_POS_COL = 0;

        // change protocol
        // TCP <-> UDP
        void Protocol_Changed(Object^ sender, RoutedEventArgs^ e);

        // change role from server client (or vice versa)
        void Role_Changed(Object^ sender, RoutedEventArgs^ e);
    };
}

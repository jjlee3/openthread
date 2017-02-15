//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "Types.h"
#include "IAsyncThreadPage.h"

namespace SocketUdpUwp
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
        static constexpr int ROLE_POS_ROW = 2;
        static constexpr int ROLE_POS_COL = 0;

        // change role from server client (or vice versa)
        void Role_Changed(Object^ sender, RoutedEventArgs^ e);

        // (server or client) control position in MainPageGrid->Children
        int roleCtrlIndex_ = -1;
    };
}

//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "IAsyncThreadPage.h"

namespace SocketUwp
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed : public IAsyncThreadPage
	{
	public:
        property static unsigned int ROLE_POS
        {
            unsigned int get() { return 1; }
        }

        MainPage();

        virtual void NotifyFromAsyncThread(Platform::String^ message, NotifyType type);

        void Notify(Platform::String^ message, NotifyType type);

    private:
        void Role_Changed(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}

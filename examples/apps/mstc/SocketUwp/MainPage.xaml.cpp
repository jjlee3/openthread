//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "ServerView.xaml.h"
#include "ClientView.xaml.h"

using namespace SocketUwp;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Core;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();

    ServerRadio->IsChecked = true;
}

void
MainPage::NotifyFromAsyncThread(
    Platform::String^ message,
    NotifyType        type)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, message, type]()
    {
        Notify(message, type);
    }));
}

void
MainPage::Notify(
    Platform::String^ message,
    NotifyType        type)
{
    switch (type)
    {
    case NotifyType::Status:
        StatusBorder->Background = ref new SolidColorBrush(Windows::UI::Colors::Green);
        break;
    case NotifyType::Error:
        StatusBorder->Background = ref new SolidColorBrush(Windows::UI::Colors::Red);
        break;
    default:
        break;
    }

    StatusBlock->Text = message;

    // Collapse the StatusBlock if it has no text to conserve real estate.
    if (StatusBlock->Text != "")
    {
        StatusBorder->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
    else
    {
        StatusBorder->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }
}

void
MainPage::Role_Changed(
    Platform::Object^                   sender,
    Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto radioBtn = dynamic_cast<RadioButton^>(sender);
    if (!radioBtn) { return; }

    UserControl^ view;

    if (radioBtn == ServerRadio)
    {
        auto serverView = ref new ServerView();
        serverView->Init(this);
        view = serverView;
    }
    else
    {
        auto clientView = ref new ClientView();
        clientView->Init(this);
        view = clientView;
    }

    auto role = RolePanel->Children->GetAt(ROLE_POS);
    if (role)
    {
    }
    else if (RolePanel->Children->Size <= ROLE_POS)
    {
        RolePanel->Children->Append(view);
        return;
    }
    else
    {
    }

    RolePanel->Children->SetAt(ROLE_POS, view);
}

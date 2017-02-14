//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "ServerControl.xaml.h"
#include "ClientControl.xaml.h"

using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

SocketTcpUwp::MainPage::MainPage()
{
	InitializeComponent();

    ServerRadio->IsChecked = true;
}

void
SocketTcpUwp::MainPage::NotifyFromAsyncThread(
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
SocketTcpUwp::MainPage::Notify(
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
SocketTcpUwp::MainPage::Role_Changed(
    Platform::Object^                   sender,
    Windows::UI::Xaml::RoutedEventArgs^ e)
{
    auto radioBtn = dynamic_cast<RadioButton^>(sender);
    if (!radioBtn) { return; }

    UserControl^ ctrl;

    if (radioBtn == ServerRadio)
    {
        auto serverCtrl = ref new ServerControl();
        serverCtrl->Init(this);
        ctrl = serverCtrl;
    }
    else
    {
        auto clientCtrl = ref new ClientControl();
        clientCtrl->Init(this);
        ctrl = clientCtrl;
    }

    auto role = RolePanel->Children->GetAt(ROLE_POS);
    if (role)
    {
    }
    else if (RolePanel->Children->Size <= ROLE_POS)
    {
        RolePanel->Children->Append(ctrl);
        return;
    }
    else
    {
    }

    RolePanel->Children->SetAt(ROLE_POS, ctrl);
}
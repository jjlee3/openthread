//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "Configurations.h"
#include "ServerControl.xaml.h"
#include "ClientControl.xaml.h"

using namespace SocketUwp;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
    InitializeComponent();

    ServerRole->Init(this);
    ClientRole->Init(this);
    TcpRadio->IsChecked = g_configurations.protocol == Protocol::TCP;
    ServerRadio->IsChecked = true;
}

void
MainPage::NotifyFromAsyncThread(
    String^    message,
    NotifyType type)
{
    Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this, message, type]()
    {
        Notify(message, type);
    }));
}

void
MainPage::Notify(
    String^    message,
    NotifyType type)
{
    switch (type)
    {
    case NotifyType::Status:
        StatusBorder->Background = ref new SolidColorBrush(Colors::Green);
        break;
    case NotifyType::Error:
        StatusBorder->Background = ref new SolidColorBrush(Colors::Red);
        break;
    default:
        break;
    }

    StatusBlock->Text = message;

    // Collapse the StatusBlock if it has no text to conserve real estate.
    if (StatusBlock->Text != "")
    {
        StatusBorder->Visibility = WUX::Visibility::Visible;
    }
    else
    {
        StatusBorder->Visibility = WUX::Visibility::Collapsed;
    }
}

void
MainPage::Protocol_Changed(
    Object^          sender,
    RoutedEventArgs^ e)
{
    auto radioBtn = dynamic_cast<RadioButton^>(sender);
    if (!radioBtn) { return; }

    if (radioBtn == TcpRadio)
    {
        g_configurations.protocol = Protocol::TCP;
    }
    else
    {
        g_configurations.protocol = Protocol::UDP;
    }
}

void
MainPage::Role_Changed(
    Object^          sender,
    RoutedEventArgs^ e)
{
    auto radioBtn = dynamic_cast<RadioButton^>(sender);
    if (!radioBtn) { return; }

    if (radioBtn == ServerRadio)
    {
        ClientRole->Visibility = WUX::Visibility::Collapsed;
        ServerRole->Visibility = WUX::Visibility::Visible;
    }
    else
    {
        ClientRole->Visibility = WUX::Visibility::Visible;
        ServerRole->Visibility = WUX::Visibility::Collapsed;
    }
}

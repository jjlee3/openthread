//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "ServerControl.xaml.h"
#include "ClientControl.xaml.h"

using namespace SocketUdpUwp;

using namespace Windows::UI::Core;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
    InitializeComponent();

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
        StatusBorder->Visibility = Windows::UI::Xaml::Visibility::Visible;
    }
    else
    {
        StatusBorder->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
    }
}

void
MainPage::Role_Changed(
    Object^          sender,
    RoutedEventArgs^ e)
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

    // remove an old control (client or server)
    if (roleCtrlIndex_ >= 0)
    {
        MainPageGrid->Children->RemoveAt(roleCtrlIndex_);
    }

    // add a new control (client or server)
    Grid::SetRow(ctrl, ROLE_POS_ROW);
    Grid::SetColumn(ctrl, ROLE_POS_COL);
    MainPageGrid->Children->Append(ctrl);

    // record the new contol index
    // to be removed at next time
    unsigned int index = -1;
    auto ok = MainPageGrid->Children->IndexOf(ctrl, &index);
    if (ok) { roleCtrlIndex_ = index; }
}

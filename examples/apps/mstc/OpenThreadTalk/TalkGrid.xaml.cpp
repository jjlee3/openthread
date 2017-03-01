//
// TalkGrid.xaml.cpp
// Implementation of the TalkGrid class
//

#include "pch.h"
#include "TalkGrid.xaml.h"
#include "Configurations.h"
#include "ServerControl.xaml.h"
#include "ClientControl.xaml.h"

using namespace OpenThreadTalk;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

TalkGrid::TalkGrid()
{
	InitializeComponent();

    TcpRadio->IsChecked = g_configurations.protocol == Protocol::TCP;
    ServerRadio->IsChecked = true;
}

void
TalkGrid::Init(
    IAsyncThreadPage^    page,
    IMainPageUIElements^ mainPageUIElements)
{
    ServerRole->Init(page, mainPageUIElements);
    ClientRole->Init(page, mainPageUIElements);
}

void
TalkGrid::Protocol_Changed(
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
TalkGrid::Role_Changed(
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

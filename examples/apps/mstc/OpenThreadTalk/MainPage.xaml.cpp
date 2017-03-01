//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "ThreadGrid.xaml.h"
#include "TalkGrid.xaml.h"

using namespace OpenThreadTalk;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
    InitializeComponent();

    ThrdGrid->Init(this, this);
    TlkGrid->Init(this, this);
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
MainPage::OnNavigatedTo(
    NavigationEventArgs^ e)
{
    Loaded += ref new RoutedEventHandler(this, &MainPage::OnLoaded);
    Unloaded += ref new RoutedEventHandler(this, &MainPage::OnUnloaded);
}

void
MainPage::OnLoaded(
    Object^          sender,
    RoutedEventArgs^ e)
{
    ThrdGrid->OnLoaded(sender, e);
}

void
MainPage::OnUnloaded(
    Object^          sender,
    RoutedEventArgs^ e)
{
    ThrdGrid->OnUnloaded(sender, e);
}

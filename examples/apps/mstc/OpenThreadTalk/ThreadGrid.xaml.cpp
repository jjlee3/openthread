//
// ThreadGrid.xaml.cpp
// Implementation of the ThreadGrid class
//

#include "pch.h"
#include "ThreadGrid.xaml.h"
#include "ThreadGlobals.h"
#include "ThreadInterface.xaml.h"

using namespace OpenThreadTalk;

// The User Control item template is documented at http://go.microsoft.com/fwlink/?LinkId=234236

ThreadGrid::ThreadGrid()
{
	InitializeComponent();
}

void
ThreadGrid::Init(
    IAsyncThreadPage^    page,
    IMainPageUIElements^ mainPageUIElements)
{
    page_ = page;
    mainPageUIElements_ = mainPageUIElements;

    ThrdInterfaceDetails->Init(page);
    ThrdInterfaceConfiguration->Init(page);

    ThreadDeviceListEvent +=
        ref new ThreadDeviceListEventHandler(
            [this]()
    {
        BuildInterfaceList();
    });

    Talk->Click +=
        ref new RoutedEventHandler([this](Object^, RoutedEventArgs^)
    {
        Visibility = WUX::Visibility::Collapsed;
        mainPageUIElements_->TalkGrid()->Visibility = WUX::Visibility::Visible;
    });
}


void
ThreadGrid::OnLoaded(
    Object^          sender,
    RoutedEventArgs^ e)
{
    try
    {
        threadGlobals_ = std::make_unique<ThreadGlobals>(page_, Dispatcher,
            [this]()
        {
            // raise a thread device list ready event
            // to notify all subscribers
            ThreadDeviceListEvent();
        });

        g_pThreadGlobals = threadGlobals_.get();
    }
    catch (Exception^ ex)
    {
        page_->NotifyFromAsyncThread("OnLoaded error: " + ex->Message,
            NotifyType::Error);
    }
}

void
ThreadGrid::OnUnloaded(
    Object^          sender,
    RoutedEventArgs^ e)
{
    g_pThreadGlobals = nullptr;
    threadGlobals_.reset();
}

void
ThreadGrid::BuildInterfaceList()
{
    InterfaceList->Items->Clear();

    auto& devices = g_pThreadGlobals->devices_;
    for (auto& device : devices)
    {
        try
        {
            InterfaceList->Items->Append(
                ThreadInterface::Create(device.GetGuid(), this));
        }
        catch (Exception^ ex)
        {
            page_->NotifyFromAsyncThread(
                "BuildInterfaceList error: " + ex->Message,
                NotifyType::Error);
        }
    }

    ThrdInterfaceDetails->BuildDeviceList();
    ThrdInterfaceConfiguration->BuildDeviceList();
}
